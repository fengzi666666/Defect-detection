#include "mainwindow.h"

//显示图片
void MainWindow::receiveData() {
    if (isCameraOpen)//传感器打开的话
    {
        //std::thread t([=]()
            //{   
                Status isCameraSuccess= STATUS_ERROR;//读取相机帧是否成功
                bool isVedioSuccess = false;//读取视频帧是否成功
                if (isCamera) {
                    pStreams = new VideoStream * [1]{ &colorStream };//[1]表示创建的是一个指针形的数组，数组的长度为1,{}大括号括出数据的成员

                    //根据时间信号读取数据
                    int changedStreamDummy;


                    //等待一帧,返回设备状态
                    isCameraSuccess = OpenNI::waitForAnyStream(pStreams, 1, &changedStreamDummy, 100);
                    if (isCameraSuccess != STATUS_OK)//等待不成功时持续等待
                    {
                        appendMsg("等待中接收数据");

                    }

                    //读取彩色帧
                    isCameraSuccess = colorStream.readFrame(&frame);//rc为读取的成功与否的状态，fream为存储彩色帧的变量
                }
                else if(isVideo){
                    
                    isVedioSuccess = vid_capture.read(frame1);//初始化一个布尔值来检查是否有帧

                }
                else if (isImage) {
                    image = cv::imread(imagePath);
                }
                
                if (isCameraSuccess == STATUS_OK||isVedioSuccess==true||!image.empty())//读取帧成功时
                {
                    if(ui.imageDetection->isChecked()){

                        double t = (double)cv::getTickCount();//开始计时
               
                        if (isCamera) {
                            //获取彩色数据
                            pColor = (RGB888Pixel*)frame.getData();
                            auto colorWidth = frame.getWidth();
                            auto colorHeight = frame.getHeight();


                            rawMat=cv::Mat(colorHeight, colorWidth, CV_8UC3, (void*)pColor);//彩色数据按BGR存储
                            cv::flip(rawMat, rawMat, 1);//水平反转
                            cv::cvtColor(rawMat, colorMat, cv::COLOR_BGR2RGB);//bgr转rgb
                        }
                        else if (isVideo) {
                            cv::resize(frame1, frame1, cv::Size(640, 480), 0, 0, cv::INTER_LINEAR);
                            rawMat = frame1;
                            cv::cvtColor(rawMat, colorMat, cv::COLOR_BGR2RGB);//bgr转rgb

                        }
                        else if (isImage) {
                            cv::resize(image, image, cv::Size(640, 480), 0, 0, cv::INTER_LINEAR);
                            rawMat = image;     
                            cv::cvtColor(rawMat, colorMat, cv::COLOR_BGR2RGB);//bgr转rgb
                        }
                        

                        //图像处理区域
                        Rect rect = Rect(Point2i(ui.Point1_X->value(), ui.Point1_Y->value()), Point2i(ui.Point2_X->value(), ui.Point2_Y->value()));
                        cv::rectangle(colorMat, rect, Scalar(255, 0, 0), 2);//画矩形
                        cv::cvtColor(colorMat, colorMat, cv::COLOR_BGR2RGB);//bgr转rgb

                        

                        Mat gray_src1;
                        cv::cvtColor(colorMat, gray_src1, cv::COLOR_BGR2GRAY);//bgr转灰度图

                        rawMat.copyTo(tailorMat);
                        tailorMat = tailorMat(rect);//裁剪出矩形区域
                        cv::cvtColor(tailorMat, tailorMat, cv::COLOR_BGR2RGB);//bgr转rgb

                        Image1 = tailorMat;//保存图

                        cv::cvtColor(tailorMat, gray_src, cv::COLOR_BGR2GRAY);//bgr转灰度图
                        cv::threshold(gray_src, binaryMat, ui.grayBox->value(), 255, 0);//60为阈值，255为最大值，0：超过60改为255.

                        Image2 = binaryMat;//保存图

                        Mat imageROI = gray_src1(rect);
                        binaryMat.copyTo(imageROI);


                        Mat canny_output;
                        vector<vector<Point>> contours;//建立二维点元素数组  全部发现的轮廓对象
                        vector<Vec4i> hierachy;//层次数组

                        Canny(gray_src, canny_output, ui.threshold_value->value(), ui.threshold_value->value() * 2, 3, false);//边缘提取
                        findContours(canny_output, contours, hierachy, RETR_EXTERNAL, CHAIN_APPROX_NONE, Point(0, 0));//发现轮廓



                        vector<Moments> contours_moments(contours.size());//矩数组
                        vector<Point2f> ccs(contours.size());//中心点坐标数组 x y
                        for (size_t i = 0; i < contours.size(); i++) {
                            contours_moments[i] = moments(contours[i]);//求矩
                            ccs[i] = Point(static_cast<float>(contours_moments[i].m10 / contours_moments[i].m00), static_cast<float>(contours_moments[i].m01 / contours_moments[i].m00));//求中心坐标  static_cast<float>处理范围
                        }

                        Mat drawImg;// = Mat::zeros(src.size(), CV_8UC3);
                        colorMat.copyTo(drawImg);
                        RNG rng(12345);//随机数
                        int P1P2_wide = (ui.Point2_X->value() - ui.Point1_X->value());//像素宽度
                        double centerPointRatio = ui.wideBox->value() / P1P2_wide;//比例
                        for (size_t i = 0; i < contours.size(); i++) {
                            if (contours[i].size() < ui.sizeBoxMin->value() || contours[i].size() > ui.sizeBoxMax->value()) {//阈值 轮廓对象小于100忽略
                                continue;
                            }
                            Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));


                            appendMsg("area " + QString::number(i) + ":" + QString::number(contourArea(contours[i])));
                            appendMsg("center point x:" + QString::number(ccs[i].x * centerPointRatio) + " y:" + QString::number(ccs[i].y * centerPointRatio));

                            //printf("center point x : %.2f y : %.2f\n", ccs[i].x+ ui.Point1_X->value(), ccs[i].y+ ui.Point1_Y->value());//打印中点坐标
                            //printf("contours %d area : %.2f\n", i, contourArea(contours[i]));//打印面积

                            drawContours(drawImg, contours, i, color, 2, 8, hierachy, 0, Point(ui.Point1_X->value(), ui.Point1_Y->value()));//绘画轮廓层次
                            circle(drawImg, Point(ccs[i].x + ui.Point1_X->value(), ccs[i].y + ui.Point1_Y->value()), 1, color, 2, 8);//绘画中点
                        }

                        Image3 = drawImg(rect);//保存图
                        //cv::cvtColor(Image3, Image3, cv::COLOR_BGR2RGB);//bgr转rgb
                        //binaryMat.convertTo(binaryMat2, CV_8UC3);

                        ////【2】定义一个Mat类型并给其设定ROI区域
                        //Mat colorMat2;
                        //colorMat.copyTo(colorMat2);
                        //Mat imageROI = colorMat2(rect);    //450，20为自定义起始点坐标
                        //binaryMat.copyTo(colorMat2, imageROI);

                        //ImgR = QImage((const uchar*)colorMat.data, colorMat.cols, colorMat.rows, QImage::Format_RGB888);
                        ImgR = QImage((const uchar*)drawImg.data, drawImg.cols, drawImg.rows, QImage::Format_RGB888);
                        QPixmap pixmap1 = QPixmap::fromImage(ImgR);
                        pixmap1 = pixmap1.scaled(pixmap1.width(), pixmap1.height());
                        //pixmap1 = pixmap1.scaled(640, 480);
                        ui.rawlabel->setPixmap(pixmap1);

                        //ImgB = QImage((const uchar*)drawImg.data, drawImg.cols, drawImg.rows, QImage::QImage::Format_RGB888);
                        ImgB = QImage((const uchar*)gray_src1.data, gray_src1.cols, gray_src1.rows, QImage::QImage::Format_Indexed8);
                        QPixmap pixmap2 = QPixmap::fromImage(ImgB);
                        pixmap2 = pixmap2.scaled(pixmap2.width(), pixmap2.height());
                        //pixmap2 = pixmap2.scaled(640, 480);
                        ui.fanllabel->setPixmap(pixmap2);
                        //ui.fanllabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
                        ui.rawlabel->setScaledContents(true);
                        ui.fanllabel->setScaledContents(true);

                        t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();//结束计时
                        double fps = 1.0 / t;//转换为帧率
                        cout << "FPS: " << fps << endl;//输出帧率 
                    }
                    else if (ui.objectDetection->isChecked()) {

                        double t = (double)cv::getTickCount();//开始计时


                        if (isCamera) {
                            //获取彩色数据
                            pColor = (RGB888Pixel*)frame.getData();
                            auto colorWidth = frame.getWidth();
                            auto colorHeight = frame.getHeight();


                            rawMat = cv::Mat(colorHeight, colorWidth, CV_8UC3, (void*)pColor);//彩色数据按BGR存储
                            cv::flip(rawMat, rawMat, 1);//水平反转
                            cv::cvtColor(rawMat, colorMat, cv::COLOR_BGR2RGB);//bgr转rgb
                        }
                        else if (isVideo) {
                            cv::resize(frame1, frame1, cv::Size(640, 480), 0, 0, cv::INTER_LINEAR);
                            rawMat = frame1;
                            cv::cvtColor(rawMat, colorMat, cv::COLOR_BGR2RGB);//bgr转rgb

                        }
                        else if (isImage) {
                            rawMat = image;
                            cv::resize(rawMat, rawMat, cv::Size(640, 480), 0, 0, cv::INTER_LINEAR);
                            cv::cvtColor(rawMat, colorMat, cv::COLOR_BGR2RGB);//bgr转rgb
                        }

                        //图像处理区域
                        Rect rect = Rect(Point2i(ui.Point1_X->value(), ui.Point1_Y->value()), Point2i(ui.Point2_X->value(), ui.Point2_Y->value()));
                        cv::rectangle(colorMat, rect, Scalar(255, 0, 0), 2);//画矩形
                        cv::cvtColor(colorMat, colorMat, cv::COLOR_BGR2RGB);//bgr转rgb

                        rawMat.copyTo(tailorMat);
                        tailorMat = tailorMat(rect);//裁剪出矩形区域
                       

                        //保存raw图
                        tailorMat.copyTo(Image1);
                        cv::cvtColor(Image1, Image1, cv::COLOR_BGR2RGB);//bgr转r
                         
                       

                        cv::cvtColor(tailorMat, tailorMat, cv::COLOR_BGR2RGB);//bgr转r
                        
                        
                    
                       
                        result = detector.detect(tailorMat, ui.confThreshold->value(), ui.iouThreshold->value());//检测单帧画面 得到Detection类型的数组
                        
                        utilss::visualizeDetection(tailorMat, result, classNames);//在画面绘出矩形框以及信息

                        Image2 = tailorMat;//保存detect检测结果图

                        Mat colorMat2;
                        colorMat.copyTo(colorMat2);
                        Mat colorMat2ROI;
                        colorMat2ROI = colorMat2(rect);
                        tailorMat.copyTo(colorMat2ROI);
                        cv::cvtColor(colorMat2, colorMat2, cv::COLOR_BGR2RGB);//bgr转r

                        //ImgR = QImage((const uchar*)colorMat.data, colorMat.cols, colorMat.rows, QImage::Format_RGB888);
                        ImgR = QImage((const uchar*)colorMat.data, colorMat.cols, colorMat.rows, QImage::Format_RGB888);
                        QPixmap pixmap1 = QPixmap::fromImage(ImgR);
                        pixmap1 = pixmap1.scaled(pixmap1.width(), pixmap1.height());
                        //pixmap1 = pixmap1.scaled(640,480);
                        ui.rawlabel->setPixmap(pixmap1);

                        //ImgB = QImage((const uchar*)drawImg.data, drawImg.cols, drawImg.rows, QImage::QImage::Format_RGB888);
                        ImgB = QImage((const uchar*)colorMat2.data, colorMat2.cols, colorMat2.rows, QImage::QImage::Format_RGB888);
                        QPixmap pixmap2 = QPixmap::fromImage(ImgB);
                        pixmap2 = pixmap2.scaled(pixmap2.width(), pixmap2.height());
                        //pixmap2 = pixmap2.scaled(640,480);
                        ui.fanllabel->setPixmap(pixmap2);
                        //ui.fanllabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
                        ui.rawlabel->setScaledContents(true);
                        ui.fanllabel->setScaledContents(true);

                        t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();//结束计时
                        double fps = 1.0 / t;//转换为帧率
                        cout << "FPS: " << fps << endl;//输出帧率 
                    }

                }
                if (isCameraSuccess != STATUS_OK&&isVedioSuccess == false&&image.empty()) {
                    this->vid_capture.release();
                    visualTime->stop();
                    isCameraOpen = false;
                    //设置开始按钮不可点击
              
                    ui.startButton->setEnabled(true);
                    ui.closeButton->setEnabled(true);
                    

                }
                if (colorMat.empty())//灰度图为空
                {
                    appendMsg("彩色图为空");
                }
                else
                {
                    emit signalJudge();
                }

                //保存当前帧
                if (isSaveVedio) {
                    outputVideo << rawMat;
                }
                
            //});
        //t.join();
    }
}