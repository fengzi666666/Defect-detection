#include "detector.h"
#include"mainwindow.h"

YOLODetector::YOLODetector(const std::string& modelPath,
                           const bool& isGPU = true,
                           const cv::Size& inputSize = cv::Size(640, 640))
{
    
    //初始化环境，每个进程一个环境
    //环境保留了线程池和其他状态信息
    env = Ort::Env(OrtLoggingLevel::ORT_LOGGING_LEVEL_WARNING, "ONNX_DETECTION");
    //初始化Session选项
    sessionOptions = Ort::SessionOptions();
    //sessionOptions.SetIntraOpNumThreads(5);//设置线程数
    std::vector<std::string> availableProviders = Ort::GetAvailableProviders();//找到可用的显卡设备
    auto cudaAvailable = std::find(availableProviders.begin(), availableProviders.end(), "CUDAExecutionProvider");//遍历设备
    //OrtCUDAProviderOptions cudaOption;

    
   
    Ort::Env env = Ort::Env{ ORT_LOGGING_LEVEL_VERBOSE, "Default" };//设置为VERBOSE，方便控制台输出时看到是使用了cpu还是gpu执行

    if (isGPU && (cudaAvailable == availableProviders.end()))//使用GPU但是找不到可使用的显卡
    {
        std::cout << "GPU is not supported by your ONNXRuntime build. Fallback to CPU." << std::endl;
        std::cout << "Inference device: CPU" << std::endl;
    }
    else if (isGPU && (cudaAvailable != availableProviders.end()))//使用GPU找到了可使用的显卡
    {   
        std::cout << "Inference device: GPU" << std::endl;
        //sessionOptions.AppendExecutionProvider_CUDA(cudaOption);
        
        // 第二个参数代表GPU device_id = 0，注释这行就是cpu执行
        Ort::ThrowOnError(OrtSessionOptionsAppendExecutionProvider_CUDA(sessionOptions, 0));
    }
    else//使用CPU
    {
        std::cout << "Inference device: CPU" << std::endl;
    }

#ifdef _WIN32
    std::wstring w_modelPath = utilss::charToWstring(modelPath.c_str());//wstring是宽字符，占用2个字节的大小，针对UNICODE编码格式
    session = Ort::Session(env, w_modelPath.c_str(), sessionOptions);//把模型加载到内存中
#else
    session = Ort::Session(env, modelPath.c_str(), sessionOptions);
#endif


    //打印模型的输入层(node names, types, shape etc.)
    Ort::AllocatorWithDefaultOptions allocator;

    // 输出输入节点的类型 维度
    Ort::TypeInfo inputTypeInfo = session.GetInputTypeInfo(0);
    std::vector<int64_t> inputTensorShape = inputTypeInfo.GetTensorTypeAndShapeInfo().GetShape();

    // checking if width and height are dynamic 检查宽度和高度是否是动态的
    this->isDynamicInputShape = false;
    if (inputTensorShape[2] == -1 && inputTensorShape[3] == -1)
    {
        std::cout << "Dynamic input shape" << std::endl;
        this->isDynamicInputShape = true;
    }


    //// 打印所有输入层信息
    //for (auto shape : inputTensorShape)
    //    std::cout << "Input shape: " << shape << std::endl;//(1,3,640,640)


    //输入输出节点名称
    inputNames.push_back(session.GetInputName(0, allocator));//image
    outputNames.push_back(session.GetOutputName(0, allocator));//output

    //std::cout << "Input name: " << inputNames[0] << std::endl;
    //std::cout << "Output name: " << outputNames[0] << std::endl;

    this->inputImageShape = cv::Size2f(inputSize);
}


//找到最佳类别的信息
void YOLODetector::getBestClassInfo(std::vector<float>::iterator it, const int& numClasses,
                                    float& bestConf, int& bestClassId)
{
    // first 5 element are box and obj confidence   前5个要素是方框和obj置信度
    bestClassId = 5;
    bestConf = 0;

    for (int i = 5; i < numClasses + 5; i++)//遍历各个类别的置信度 找到置信度最大的类别
    {
        if (it[i] > bestConf)
        {
            bestConf = it[i];
            bestClassId = i - 5;
        }
    }

}

//标准化处理  归一化
void YOLODetector::preprocessing(cv::Mat &image, float*& blob, std::vector<int64_t>& inputTensorShape)
{
    cv::Mat resizedImage, floatImage;
    cv::cvtColor(image, resizedImage, cv::COLOR_BGR2RGB);//转换色彩空间 BGR to RGB
    utilss::letterbox(resizedImage, resizedImage, this->inputImageShape,
                     cv::Scalar(114, 114, 114), this->isDynamicInputShape,
                     false, true, 32);// 填充调整图片大小

    inputTensorShape[2] = resizedImage.rows;//高
    inputTensorShape[3] = resizedImage.cols;//宽

    resizedImage.convertTo(floatImage, CV_32FC3, 1 / 255.0);
    blob = new float[floatImage.cols * floatImage.rows * floatImage.channels()];//宽*高*通道
    cv::Size floatImageSize {floatImage.cols, floatImage.rows};//图片大小{宽，高}

    // hwc -> chw   高宽通道转换为通道高宽
    std::vector<cv::Mat> chw(floatImage.channels());
    for (int i = 0; i < floatImage.channels(); ++i)//遍历3通道
    {
        chw[i] = cv::Mat(floatImageSize, CV_32FC1, blob + i * floatImageSize.width * floatImageSize.height);
    }
    cv::split(floatImage, chw); //通道分离
}

//后处理
std::vector<Detection> YOLODetector::postprocessing(const cv::Size& resizedImageShape,
                                                    const cv::Size& originalImageShape,
                                                    std::vector<Ort::Value>& outputTensors,
                                                    const float& confThreshold, const float& iouThreshold)
{
    std::vector<cv::Rect> boxes;//目标框坐标
    std::vector<float> confs;//置信度
    std::vector<int> classIds;//类别ID

    auto* rawOutput = outputTensors[0].GetTensorData<float>();
    std::vector<int64_t> outputShape = outputTensors[0].GetTensorTypeAndShapeInfo().GetShape();
    size_t count = outputTensors[0].GetTensorTypeAndShapeInfo().GetElementCount();
    std::vector<float> output(rawOutput, rawOutput + count);

    // for (const int64_t& shape : outputShape)
    //     std::cout << "Output Shape: " << shape << std::endl;

    // first 5 elements are box[4] and obj confidence  前5个要素是方框4个坐标和目标置信度
    int numClasses = (int)outputShape[2] - 5;
    int elementsInBatch = (int)(outputShape[1] * outputShape[2]);

    // only for batch size = 1  仅适用于批量大小=1
    for (auto it = output.begin(); it != output.begin() + elementsInBatch; it += outputShape[2])
    {
        float clsConf = it[4];

        if (clsConf > confThreshold)
        {
            int centerX = (int) (it[0]);
            int centerY = (int) (it[1]);
            int width = (int) (it[2]);
            int height = (int) (it[3]);
            int left = centerX - width / 2;
            int top = centerY - height / 2;

            float objConf;
            int classId;
            this->getBestClassInfo(it, numClasses, objConf, classId);

            float confidence = clsConf * objConf;

            boxes.emplace_back(left, top, width, height);
            confs.emplace_back(confidence);
            classIds.emplace_back(classId);
        }
    }


   // NMS  执行非最大抑制，以消除冗余重叠框 
    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confs, confThreshold, iouThreshold, indices);
    // std::cout << "amount of NMS indices: " << indices.size() << std::endl;

    std::vector<Detection> detections;

    for (int idx : indices)
    {
        Detection det;
        det.box = cv::Rect(boxes[idx]);
        utilss::scaleCoords(resizedImageShape, det.box, originalImageShape);//缩放坐标 

        det.conf = confs[idx];
        det.classId = classIds[idx];
        detections.emplace_back(det);
    }

    return detections;
}

std::vector<Detection> YOLODetector::detect(cv::Mat &image, const float& confThreshold = 0.4,
                                            const float& iouThreshold = 0.45)
{
    float *blob = nullptr;
    std::vector<int64_t> inputTensorShape {1, 3, -1, -1};
    this->preprocessing(image, blob, inputTensorShape);//标准化处理  归一化

    size_t inputTensorSize = utilss::vectorProduct(inputTensorShape);

    std::vector<float> inputTensorValues(blob, blob + inputTensorSize);

    std::vector<Ort::Value> inputTensors;


    //创建输入tensor
    Ort::MemoryInfo memoryInfo = Ort::MemoryInfo::CreateCpu(
            OrtAllocatorType::OrtArenaAllocator, OrtMemType::OrtMemTypeDefault);

    inputTensors.push_back(Ort::Value::CreateTensor<float>(
            memoryInfo, inputTensorValues.data(), inputTensorSize,
            inputTensorShape.data(), inputTensorShape.size()
    ));


    //推理(score model & input tensor, get back output tensor)
    std::vector<Ort::Value> outputTensors = this->session.Run(Ort::RunOptions{nullptr},
                                                              inputNames.data(),
                                                              inputTensors.data(),
                                                              1,
                                                              outputNames.data(),
                                                              1);

    cv::Size resizedShape = cv::Size((int)inputTensorShape[3], (int)inputTensorShape[2]);
    std::vector<Detection> result = this->postprocessing(resizedShape,
                                                         image.size(),
                                                         outputTensors,
                                                         confThreshold, iouThreshold);//后处理

    delete[] blob;

    return result;
}
