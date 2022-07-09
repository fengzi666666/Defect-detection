#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    pDlg = new setDialog(this);//初始化对话窗口
    pDlg1 = new outputImageDialog(this);
    pDlg2 = new outputVedioDialog(this);
    outputVideo = VideoWriter();//视频录制
    ////////////////////////////////主界面设置//////////////////////////////////////

    this->setFixedSize(1205, 747);
    this->setWindowIcon(QIcon(":/res/title.png"));
    this->setWindowTitle("缺陷检测系统");


    //设置参数默认值
    ui.grayBox->setValue(85.00);
    ui.numBox->setValue(20);
    ui.portBoxOUT->setValue(0);
    ui.portBoxIN->setValue(0);
    ui.IP->setText("192.168.1.2");
    ui.Port->setText("502");
    ui.Point1_X->setValue(30);
    ui.Point1_Y->setValue(30);
    ui.Point2_X->setValue(620);
    ui.Point2_Y->setValue(450);
    ui.threshold_value->setValue(80);
    ui.wideBox->setValue(1070);
    ui.weightChose_2->setText("weights1/unchange_best.onnx");
    ui.labelChose_2->setText("class2.names");

    ui.confThreshold->setValue(0.25);
    ui.iouThreshold->setValue(0.45);
    //设置参数控件的上限值
    ui.grayBox->setMaximum(255.00);//灰度值  Spin Box
    ui.numBox->setMaximum(768000);


    ui.wideBox->setSuffix("mm");
    //设置状态指示灯为绿色
    //QPixmap light = QPixmap(":/res/state_red_48.png");
    //ui.lightlabel->setPixmap(light);

 


    
    //ui.widgetTitle->setProperty("form", "title");
    ////ui.widgetTop->setProperty("nav", "top");
    //ui.labTitle->setText("智能访客管理平台");
    //ui.labTitle->setFont(QFont("Microsoft Yahei", 20));
    //this->setWindowTitle(ui.labTitle->text());


    visualTime = new QTimer(this);//创建定时器
    modbusDevice = new QModbusTcpClient(this);//创建QModbusDevice对象
    connect(modbusDevice, &QModbusClient::errorOccurred, [this](QModbusDevice::Error) {
        appendMsg("ERROR: "+ modbusDevice->errorString());
        //qDebug() << "new Error: " << modbusDevice->errorString();
    });//错误提示

    connect(modbusDevice, &QModbusClient::stateChanged,this, &MainWindow::onStateChanged);//状态转变
 
    //connect(modbusDevice, &QModbusClient::stateChanged,this, &MainWindow::onStateChanged);
    //connect(myClient, &QModbusClient::stateChanged, this,&MainWindow::onStateChanged);//连接状态发生改变时处理函数
    //connect(ui.connectButton, &QPushButton::clicked, [=]() {
    //    this->connectPLC();
    //    });

    connect(this, &MainWindow::signalOpen, this, [=]() {
        
        initGPU = ui.GPU->isChecked();//初始化是否使用GPU
        //创建目标检测对象
        classNamesPath = ui.labelChose_2->text().toLocal8Bit();
        classNames = utilss::loadNames(classNamesPath);
        modelPath = ui.weightChose_2->text().toLocal8Bit();
        detector = YOLODetector(modelPath, ui.GPU->isChecked(), cv::Size(640, 640));
        visualTime->start(40);
        });//打开相机signalOpen  开启定时器
    connect(visualTime, &QTimer::timeout, this, &MainWindow::receiveData);//显示图片
    //connect(this, &MainWindow::signalJudge, this, &MainWindow::judgeFlaw);//判断缺陷
    //connect(this, &MainWindow::signalFlaw, this, &MainWindow::saveDataFile);//保存问题图片
    //connect(this, &MainWindow::signalFlaw, this, &MainWindow::changeState);//有缺陷状态提示
    //connect(this, &MainWindow::signalHandled, this, &MainWindow::changeState);//处理过问题提示

    connect(this, &MainWindow::signalWrite, this, &MainWindow::writeRequst);
    connect(this, &MainWindow::signalRead, this, &MainWindow::readCoils);

    //测试
    connect(ui.readButton, &QPushButton::clicked, this, &MainWindow::readCoils);
    connect(ui.writeButton, &QPushButton::clicked, this, &MainWindow::writeRequst);
}

////消息框
//void MainWindow::appendMsg(QString str)
//{
//    QDateTime dateTime(QDateTime::currentDateTime());//获取当前时间
//    QString qStr = dateTime.toString("yy/MM/dd hh:mm:ss");//转字符串
//    ui.textEdit->append(qStr + ": " + str);
//}


////连接状态发生改变时处理函数（connect or discennect）
//void MainWindow::onStateChanged()
//{
//    if (myClient->state() == QModbusDevice::UnconnectedState)
//    {
//        isConnected = false;
//        appendMsg("连接失败");
//        //emit updateCount("0,0");
//
//    }
//    else if (myClient->state() == QModbusDevice::ConnectedState)
//    {
//        isConnected = true;
//        appendMsg("连接成功");
//    }
//    //appendMsg(QString::number(myClient->state()));//2表示连接成功
//}
//连接硬件PLC
//void MainWindow::on_connectPLC_clicked()
//{
//    QString IP = ui.IP->text();//获取IP地址
//    QString Port = ui.Port->text();//获取端口
//    std::thread t([=]() {
//    if (!myClient)//当没有创建连接对象时
//    {
//        appendMsg("Modbus模块建立出错");
//        return;
//    }
//    if (myClient->state() == QModbusDevice::UnconnectedState)//没有连接设备时进行连接
//    {
//        appendMsg("开始连接");
//        //const QUrl url = QUrl::fromUserInput("192.168.1.1:502"); //;//获取IP和端口号
//        myClient->setConnectionParameter(QModbusDevice::NetworkAddressParameter, IP);
//        myClient->setConnectionParameter(QModbusDevice::NetworkPortParameter, Port);
//
//        myClient->setTimeout(500);//超时时间内没有相应，则设置TimeoutError
//        myClient->setNumberOfRetries(3);//请求失败前客户端执行重试的次数
//        if (!myClient->connectDevice())//连接设备
//        {
//            appendMsg("设置连接失败");
//        }
//        else
//        {
//            appendMsg("设置连接成功");
//        }
//    }
//    if (myClient->state() == QModbusDevice::ConnectedState)
//    {
//        isConnected = false;
//        myClient->disconnectDevice();//断开连接
//
//    }
//    appendMsg(QString::number(myClient->state()));//2表示连接成功
//});
//t.join();
//}

////打开相机
//void MainWindow::on_startButton_clicked()//打开相机
//{
//    if (this->isConnected == false)
//    {
//        QMessageBox::critical(this, "错误提示框", "请先进行硬件连接\n");
//    }
//    else
//    {
//        this->openCamera();
//        //设置开始按钮不可点击
//        ui.startButton->setEnabled(false);
//        ui.closeButton->setEnabled(true);
//    }
//}

////关闭相机
//void MainWindow::on_closeButton_clicked()//关闭相机
//{
//    if (this->isConnected == false)
//    {
//        QMessageBox::critical(this, "错误提示框", "请先进行硬件连接\n");
//    }
//    else
//    {
//        this->closeCamera();
//        //设置开始按钮不可点击
//        ui.startButton->setEnabled(true);
//        ui.closeButton->setEnabled(false);
//    }
//}
//打开相机函数
//void MainWindow::openCamera()//打开相机函数
//{
//    std::thread t([=]()
//        {
//            if (!isCameraOpen)
//            {
//                //初始化OpenNI SDK
//                if ((status = OpenNI::initialize()) == STATUS_OK)//表示初始化成功
//                {
//                    appendMsg("初始化成功");
//                }
//                else
//                {
//                    appendMsg("初始化失败");
//                }
//
//                //枚举设备
//                OpenNI::enumerateDevices(&deviceInfoList);
//
//                //任意openni的设备，open的参数为const char*，传递设备的uri来选择特定设备
//                if ((status = device.open(ANY_DEVICE)) == STATUS_OK)
//                {
//                    appendMsg("打开设备成功");
//                }
//                else
//                {
//                    appendMsg("打开设备失败");
//                }
//                //创建色彩流
//                colorStream.create(device, SENSOR_COLOR);
//
//                //设置彩色流的模式
//                colorMode.setPixelFormat(PIXEL_FORMAT_RGB888);//彩色流的格式
//                colorMode.setResolution(640, 480);//设置分辨率,越小所占内存越小
//                colorMode.setFps(30);//一秒拍摄15帧
//                colorStream.setVideoMode(colorMode);
//
//                //打开彩色流
//                colorStream.start();
//                appendMsg("相机开启");
//
//                isCameraOpen = true;
//                emit signalOpen();
//            }
//        });
//    t.join();
//    return;
//}
//关闭相机函数
//void MainWindow::closeCamera()//关闭相机函数
//{
//    if (isCameraOpen)
//    {
//        delete[] pStreams;
//
//        //停止彩色流
//        colorStream.stop();
//
//        //销毁彩色流
//        colorStream.destroy();
//
//        //关闭设备
//        device.close();
//
//        //释放OpenNI资源
//        OpenNI::shutdown();//关闭所有驱动并且正确地清除所有
//
//        appendMsg("------相机关闭------");
//
//        isCameraOpen = false;
//        //设置开始按钮不可点击
//        ui.startButton->setEnabled(true);
//        ui.closeButton->setEnabled(false);
//    }
//    return;
//}
////显示图片
//void MainWindow::receiveData() {
//    if (isCameraOpen)//传感器打开的话
//    {
//        std::thread t([=]()
//            {   
//                pStreams = new VideoStream * [1]{ &colorStream };//[1]表示创建的是一个指针形的数组，数组的长度为1,{}大括号括出数据的成员
//
//                //根据时间信号读取数据
//                int changedStreamDummy;
//
//                //等待一帧,返回设备状态
//                Status rc = OpenNI::waitForAnyStream(pStreams, 1, &changedStreamDummy, 100);
//                if (rc != STATUS_OK)//等待不成功时持续等待
//                {
//                    appendMsg("等待中接收数据");
//                }
//
//                //读取彩色帧
//                rc = colorStream.readFrame(&frame);//rc为读取的成功与否的状态，fream为存储彩色帧的变量
//                if (rc == STATUS_OK)//读取帧成功时
//                {
//                    //获取彩色数据
//                    pColor = (RGB888Pixel*)frame.getData();
//                    auto colorWidth = frame.getWidth();
//                    auto colorHeight = frame.getHeight();
//
//                    cv::Mat rawMat(colorHeight, colorWidth, CV_8UC3, (void*)pColor);//彩色数据按BGR存储
//                    cv::flip(rawMat, rawMat, 1);//水平反转
//                    cv::cvtColor(rawMat, colorMat, cv::COLOR_BGR2RGB);//bgr转rgb
//                  
//                    //图像处理区域
//                    Rect rect = Rect(Point2i(ui.Point1_X->value(), ui.Point1_Y->value()), Point2i(ui.Point2_X->value(), ui.Point2_Y->value()));
//                    cv::rectangle(colorMat, rect,Scalar(255, 0, 0), 2);//画矩形
//                    cv::cvtColor(colorMat, colorMat, cv::COLOR_BGR2RGB);//bgr转rgb
//
//                    Mat gray_src1;
//                    cv::cvtColor(colorMat, gray_src1, cv::COLOR_BGR2GRAY);//bgr转灰度图
//
//                    rawMat.copyTo(tailorMat);
//                    tailorMat=tailorMat(rect);//裁剪出矩形区域
//                    cv::cvtColor(tailorMat, tailorMat, cv::COLOR_BGR2RGB);//bgr转rgb
//
//                    
//                    cv::cvtColor(tailorMat, gray_src, cv::COLOR_BGR2GRAY);//bgr转灰度图
//                    cv::threshold(gray_src, binaryMat, ui.grayBox->value(), 255, 0);//60为阈值，255为最大值，0：超过60改为255.
//                    
//                    Mat imageROI = gray_src1(rect);
//                    binaryMat.copyTo(imageROI);
//
//
//                    Mat canny_output;
//                    vector<vector<Point>> contours;//建立二维点元素数组  全部发现的轮廓对象
//                    vector<Vec4i> hierachy;//层次数组
//
//                    Canny(gray_src, canny_output, ui.threshold_value->value(), ui.threshold_value->value() * 2, 3, false);//边缘提取
//                    findContours(canny_output, contours, hierachy, RETR_EXTERNAL, CHAIN_APPROX_NONE, Point(0, 0));//发现轮廓
//
//                    vector<Moments> contours_moments(contours.size());//矩数组
//                    vector<Point2f> ccs(contours.size());//中心点坐标数组 x y
//                    for (size_t i = 0; i < contours.size(); i++) {
//                        contours_moments[i] = moments(contours[i]);//求矩
//                        ccs[i] = Point(static_cast<float>(contours_moments[i].m10 / contours_moments[i].m00), static_cast<float>(contours_moments[i].m01 / contours_moments[i].m00));//求中心坐标  static_cast<float>处理范围
//                    }
//
//                    Mat drawImg;// = Mat::zeros(src.size(), CV_8UC3);
//                    colorMat.copyTo(drawImg);
//                    RNG rng(12345);//随机数
//                    int P1P2_wide=(ui.Point2_X->value() - ui.Point1_X->value());//像素宽度
//                    double centerPointRatio = ui.wideBox->value() / P1P2_wide;//比例
//                    for (size_t i = 0; i < contours.size(); i++) {
//                        if (contours[i].size() < 1000) {//阈值 轮廓对象小于100忽略
//                            continue;
//                        }
//                        Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
//
//
//                        appendMsg("area "+ QString::number(i)+":"+ QString::number(contourArea(contours[i])));
//                        appendMsg("center point x:"+ QString::number(ccs[i].x * centerPointRatio)+" y:"+ QString::number(ccs[i].y*centerPointRatio));
//                        
//                        //printf("center point x : %.2f y : %.2f\n", ccs[i].x+ ui.Point1_X->value(), ccs[i].y+ ui.Point1_Y->value());//打印中点坐标
//                        //printf("contours %d area : %.2f\n", i, contourArea(contours[i]));//打印面积
//                       
//                        drawContours(drawImg, contours, i, color, 2, 8, hierachy, 0, Point(ui.Point1_X->value(), ui.Point1_Y->value()));//绘画轮廓层次
//                        circle(drawImg,Point(ccs[i].x + ui.Point1_X->value(), ccs[i].y + ui.Point1_Y->value()), 1, color, 2, 8);//绘画中点
//                    }
//
//                    //binaryMat.convertTo(binaryMat2, CV_8UC3);
//
//                    ////【2】定义一个Mat类型并给其设定ROI区域
//                    //Mat colorMat2;
//                    //colorMat.copyTo(colorMat2);
//                    //Mat imageROI = colorMat2(rect);    //450，20为自定义起始点坐标
//                    //binaryMat.copyTo(colorMat2, imageROI);
//
//                    //ImgR = QImage((const uchar*)colorMat.data, colorMat.cols, colorMat.rows, QImage::Format_RGB888);
//                    ImgR = QImage((const uchar*)drawImg.data, drawImg.cols, drawImg.rows, QImage::Format_RGB888);
//                    QPixmap pixmap1 = QPixmap::fromImage(ImgR);
//                    pixmap1 = pixmap1.scaled(pixmap1.width() , pixmap1.height() );
//                    ui.rawlabel->setPixmap(pixmap1);
//
//                    //ImgB = QImage((const uchar*)drawImg.data, drawImg.cols, drawImg.rows, QImage::QImage::Format_RGB888);
//                    ImgB = QImage((const uchar*)gray_src1.data, gray_src1.cols, gray_src1.rows, QImage::QImage::Format_Indexed8);
//                    QPixmap pixmap2 = QPixmap::fromImage(ImgB);
//                    pixmap2 = pixmap2.scaled(pixmap2.width() , pixmap2.height() );
//                    ui.fanllabel->setPixmap(pixmap2);
//                    //ui.fanllabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
//                    ui.rawlabel->setScaledContents(true);
//                    ui.fanllabel->setScaledContents(true);
//                }
//                if (colorMat.empty())//灰度图为空
//                {
//                    appendMsg("彩色图为空");
//                }
//                else
//                {
//                    emit signalJudge();
//                }
//            });
//        t.join();
//    }
//}
////判断缺陷
//void MainWindow::judgeFlaw() {
//    std::thread t([=]() {
//        //判断每一个像素点的灰度值是否在阈值
//        int height = binaryMat.rows;
//        int width = binaryMat.cols;
//        int channels = binaryMat.channels();
//        int numberOfBlack = 0;
//
//        for (int i = 0; i < height; i++)
//        {
//            uchar* data = binaryMat.ptr<uchar>(i);//data指向图像中第i+1行的第一个元素
//            for (int j = 0; j < width; j++)
//            {
//                if (data[j] == 0) //二值图中黑色的部分
//                {
//                    numberOfBlack++;//黑色的元素的个数
//                }
//            }
//        }
//        if (numberOfBlack > ui.numBox->value())
//        {
//            appendMsg("有漏洞缺陷");
//            //改变指示灯颜色为红色
//            this->isHaveFlaw = true;
//            g_imagecount++;
//            //保存缺陷数据
//            emit signalFlaw();
//            //发送信号
//            if (isConnected == true)
//            {
//                //emit signalRead();
//                emit signalWrite();
//            }
//        }
//        });
//    t.join();
//}
//保存缺陷图片
//void MainWindow::saveDataFile()
//{
//    std::thread t([=]() {
//        //选择一个路径
//        QDateTime dateTime(QDateTime::currentDateTime());//构造函数
//        QString qStr = dateTime.toString("yy-MM-dd-hh-mm-ss");
//        QString filePath = "C:/hole_detection/dataset/" + qStr + "_" + QString::number(g_imagecount) + ".bmp";
//        cv::imwrite(filePath.toStdString(), binaryMat);//将彩色图保存到本地
//        //qDebug() << colorMat.rows;
//        appendMsg("缺陷数据保存完成,查看:C:/hole_detection/dataset");
//        });
//    t.join();
//}
//改变指示灯的状态
//void MainWindow::changeState()
//{
//    QString lightPath = NULL;
//    //有缺陷提示
//    if (this->isHaveFlaw == true)
//    {
//        lightPath = ":/res/state_red_48.png";
//    }
//    //处理过缺陷恢复原来状态
//    if (this->isHandled == true)
//    {
//        lightPath = ":/res/state_green_48.png";
//    }
//    QPixmap light = QPixmap(lightPath);
//    ui.lightlabel->setPixmap(light);
//}
////给PLC写数据s
//void MainWindow::writeRequst()
//{
//    int star_add = ui.portBoxOUT->value();//写入数据的起始地址 
//    quint16 value = 0;//要写入的数据
//    quint16 number = 1;//写入数据的个数
//    if (!myClient)
//    {
//        appendMsg("Modbus模块建立出错");
//        return;
//    }
//    //quint16 number1 = static_cast<quint16>(number);
//    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::Coils, star_add, number);//参数：寄存器类型，数据起始地址，数据大小
//    //int a = writeUnit.valueCount();
//    //for (uint i = 0; i < writeUnit.valueCount(); i++)//循环写入
//    //{
//        //int ii = static_cast<int>(i);//将uint类型数值静态转换为int型。
//        //QString st = str1.mid(ii, 1);//获取指定位置子串,参数1从哪个开始，参数2一共几位
//    writeUnit.setValue(0, value);//将用户输出的数据设置到写容器中
//    //quint16 a = writeUnit.value(0);
//    if (auto* reply = myClient->sendWriteRequest(writeUnit, 0xFF))//1为服务器的serveAddress
//    {
//        if (!reply->isFinished())//返回未终止时
//        {
//            connect(reply, &QModbusReply::finished, this, [=]() {
//                if (reply->error() == QModbusDevice::ProtocolError)
//                {
//                    QString errorM1 = QString::fromLocal8Bit("Write response error: %1 (Mobus exception: 0x%2)").arg(reply->errorString()).arg(reply->rawResult().exceptionCode(), -1, 16);
//                    appendMsg(errorM1);
//                }
//                else if (reply->error() != QModbusDevice::NoError)//错误
//                {
//                    QString errorM2 = QString::fromLocal8Bit("Write response error: %1 (code: 0x%2)").arg(reply->errorString()).arg(reply->error(), -1, 16);
//                    appendMsg(errorM2);
//                }
//                appendMsg("写入成功" + QString::number(value));
//                emit signalRead();
//                reply->deleteLater();
//                });
//        }
//        else
//        {
//            reply->deleteLater();// broadcast replies return immediately
//        }
//    }
//    else
//    {
//        QString errorM3 = ("Write error: ") + myClient->errorString();
//        appendMsg(errorM3);
//    }
//}
////读取写的内容
//bool MainWindow::readCoils()
//{
//    int start_add = ui.portBoxIN->value();//待读取的线圈的地址
//    quint16 numbers = 1;//读一位
//    if (!myClient->state() == QModbusDevice::ConnectedState)
//    {
//        return false;
//    }
//
//    QModbusDataUnit readUnit(QModbusDataUnit::Coils, start_add, numbers);
//    if (QModbusReply* reply = myClient->sendReadRequest(readUnit, 0xFF))
//    {
//        //appendMsg("serverAddress: " + QString::number(reply->serverAddress()));
//        //bool a = reply->isFinished();
//        if (!reply->isFinished())//未返回结束
//        {
//            connect(reply, &QModbusReply::finished, this, &MainWindow::readRead_coils);
//            return true;
//        }
//        else
//        {
//            appendMsg("应答结束");
//            delete reply;
//            return false;
//        }
//    }
//    else
//    {
//        appendMsg("发送读请求失败");
//        return false;
//    }
//    return false;
//}

////接收到读取请求后执行的槽函数
//void MainWindow::readRead_coils()
//{
//    //QModbusReply这个类存储了来自client的数据,sender()返回发送信号的对象的指针
//    QModbusReply* reply = qobject_cast<QModbusReply*>(sender());
//    if (!reply)
//    {
//        return;
//    }
//    if (reply->error() == QModbusDevice::NoError)
//    {
//        //处理成功返回的数据
//        const QModbusDataUnit unit = reply->result();
//        quint16 res = unit.value(0);  //读取第一位的数据
//        if (res == 1)
//        {
//            appendMsg("读取成功" + QString::number(res));
//            //设置指示灯状态为绿色
//            this->isHandled = true;
//            emit this->signalHandled();
//        }
//    }
//    else if (reply->error() == QModbusDevice::ProtocolError)//协议错误
//    {
//        QString errorR1 = QString::fromLocal8Bit("Read response error: %1 (Mobus exception: 0x%2)").arg(reply->errorString()).arg(reply->rawResult().exceptionCode(), -1, 16);
//        appendMsg(errorR1);
//    }
//    else
//    {
//        QString errorM2 = QString::fromLocal8Bit("Read response error: %1 (code: 0x%2)").arg(reply->errorString()).arg(reply->error(), -1, 16);
//        appendMsg(errorM2);
//    }
//    reply->deleteLater();
//}