#pragma once
#pragma execution_character_set("utf-8")
#include <QtWidgets/QMainWindow>
#include "ui_mainwindow.h"
#include<opencv2/opencv.hpp>
#include<OpenNI.h>
#include<qpixmap.h>
#include<qdatetime.h>
#include<qtimer.h>
#include <QtSerialBus/QModbusTcpClient>
#include <QtSerialBus/QModbusDataUnit>
#include<qmessagebox.h>
#include<qdebug.h>
#include"detector.h"
#include"utils.h"
#include"setdialog.h"
#include"outputimagedialog.h"
#include"outputvediodialog.h"
#include< QFileDialog>
#include <QString>

using namespace openni;
using namespace cv;
using namespace std;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = Q_NULLPTR);

    QModbusClient* modbusDevice = NULL;//创建一个发送请求的对象
    QTimer* visualTime = NULL;//QTimer构造
    Status status= STATUS_ERROR;//状态（类似于int类型）
    Array<DeviceInfo> deviceInfoList;//存储设备信息的array
    Device device;//创建设备 Device可以访问流（Streams）
    VideoStream colorStream;//创建色彩流
    VideoMode colorMode;//设置彩色流的模式
    VideoFrameRef frame;//创建一个Ref类型的变量，用以存储帧数据
    VideoStream** pStreams = NULL;//创建VideoStream类型的指针，指向已创建的深度流和色彩流的地址（同时获取多个流数据）
    RGB888Pixel* pColor = NULL;//用于存储获得的彩色数据
    Mat frame1;

    QString timeStr;//时间字符串
    QString savePath= "E:/programs data/QT/Defect detection/detection_data/";//保存路径
    int g_imagecount = 1;//计数,创建文件的名字



    //创建目标检测对象

    //视频推理
    bool isGPU;//设置是否使用GPU
    bool initGPU;
    std::string classNamesPath;
    std::vector<std::string> classNames;//加载类型文件，转换为标签字符串数组
    std::string modelPath;//模型地址


    //检测对象选择
    VideoCapture vid_capture;//创建一个视频捕捉对象
    bool isCamera=true;//摄像头检测
    bool isVideo = false;//视频检测
    bool isImage = false;//图片检测
    std::string vedioPath;//视频路径
    std::string imagePath;//图片路径
    bool isResolvingPower = true;//分辨率  640x480

    //图片保存设置
    bool isSaveImage1 = true;
    bool isSaveImage2 = true;
    bool isSaveImage3 = true;
    QString imageSavePath = "E:/programs data/QT/Defect detection/detection_data/imageSave";
    Mat Image1;
    Mat Image2;
    Mat Image3;
    //视频保存设置
    bool isSaveVedioLeft = true;
    bool isSaveVedioRight = true;
    QString videoSavePath= "E:/programs data/QT/Defect detection/detection_data/vedioSave/test.mp4";
    VideoWriter outputVideo;
    bool isSaveVedio = false;


    //创建检测对象
    YOLODetector detector{ nullptr };
    std::vector<Detection> result;


    void appendMsg(QString str);//提示框


private:
    Ui::MainWindowClass ui;
    setDialog* pDlg;//检测对话框指针
    outputImageDialog* pDlg1;//图片保存对话框
    outputVedioDialog* pDlg2;//视频保存对话框
    

    bool isCameraOpen = false;//摄像头连接判断
    bool isConnected = true;//PLC连接判断
    bool isHaveFlaw = false;//缺陷判断
    bool isHandled = false;
    cv::Mat image;
    cv::Mat rawMat;
    cv::Mat colorMat;//RGB图
    cv::Mat tailorMat;//裁剪图
    cv::Mat gray_src;//灰度图
    cv::Mat binaryMat;//二值图
    QImage ImgR;//RGB图Qimage绘图设备 可以对像素进行访问
    QImage ImgB;//二值图Qimage绘图设备 可以对像素进行访问

    
    
    void openCamera();//打开相机函数
    void closeCamera();//关闭相机函数
    void receiveData();//显示图片
    void judgeFlaw();//判断缺陷


public slots:
    void onStateChanged(int state);//连接状态发生改变时处理函数（connect or discennect）
    void on_connectPLC_clicked();//连接PLC
    void on_startButton_clicked();//打开相机
    void on_closeButton_clicked();//关闭相机
    void saveDataFile();//保存文件
    void changeState();//改变指示灯的状态(发现缺陷 和处理过缺陷)
    void writeRequst();//给PLC写数据
    void readReady_coils();//接收到读取请求后执行的槽函数
    bool readCoils();//读取写的内容
    void on_weightChoose_clicked();//选择权重
    void on_labelChoose_clicked();//选择文件标签
    void on_setupdialog_clicked();//设置对话框
    void on_objectDetection_clicked();//目标检测被点击
    void on_imageDetection_clicked();//图像检测被点击

    //保存设置
    void  on_imageSaveSet_clicked();
    void  on_vedioSaveSet_clicked();
    void  on_imageSave_clicked();
    void  on_vedioSave_clicked();


signals:
    void signalOpen();
    void signalJudge();
    void signalFlaw();
    void signalSaveData();
    void signalWrite();
    void signalRead();
    void signalHandled();
};


