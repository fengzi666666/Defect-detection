#pragma once
#include <opencv2/opencv.hpp>
#include <onnxruntime_cxx_api.h>
#include <utility>
#include <cuda_provider_factory.h>
#include "utils.h"


class YOLODetector
{
public:
    explicit YOLODetector(std::nullptr_t) {};//构造函数 explicit关键字用来修饰类的构造函数，不能发生相应的隐式类型转换
    YOLODetector(const std::string& modelPath,
                 const bool& isGPU,
                 const cv::Size& inputSize);//构造函数

    std::vector<Detection> detect(cv::Mat &image, const float& confThreshold, const float& iouThreshold);//推理

private:
    Ort::Env env{nullptr};
    Ort::SessionOptions sessionOptions{nullptr};
    Ort::Session session{nullptr};

    void preprocessing(cv::Mat &image, float*& blob, std::vector<int64_t>& inputTensorShape);//标准化处理  归一化
    std::vector<Detection> postprocessing(const cv::Size& resizedImageShape,
                                          const cv::Size& originalImageShape,
                                          std::vector<Ort::Value>& outputTensors,
                                          const float& confThreshold, const float& iouThreshold);//后处理

    static void getBestClassInfo(std::vector<float>::iterator it, const int& numClasses,
                                 float& bestConf, int& bestClassId);//找到最佳类别的信息

    std::vector<const char*> inputNames;
    std::vector<const char*> outputNames;
    bool isDynamicInputShape{};
    cv::Size2f inputImageShape;

};