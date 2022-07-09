#include "mainwindow.h"
//打开相机函数
void MainWindow::openCamera()//打开相机函数
{
    //std::thread t([=]()
    //    {
            if (!isCameraOpen)
            {
                //初始化OpenNI SDK
                if ((status = OpenNI::initialize()) == STATUS_OK)//表示初始化成功
                {
                    //appendMsg("初始化成功");
                    //枚举设备
                    OpenNI::enumerateDevices(&deviceInfoList);

                    //任意openni的设备，open的参数为const char*，传递设备的uri来选择特定设备
                    if ((status = device.open(ANY_DEVICE)) == STATUS_OK)
                    {
                        appendMsg("打开设备成功");
                        ui.setupdialog->setEnabled(false);
                        ui.startButton->setEnabled(false);
                        ui.closeButton->setEnabled(true);
                        //创建色彩流
                        colorStream.create(device, SENSOR_COLOR);

                        //设置彩色流的模式
                        colorMode.setPixelFormat(PIXEL_FORMAT_RGB888);//彩色流的格式

                        
                        if (isResolvingPower) {
                            colorMode.setResolution(640, 480);//设置分辨率,越小所占内存越小
                        }
                        else {
                            colorMode.setResolution(1280, 960);//设置分辨率,越小所占内存越小
                        }
                        

                        colorMode.setFps(30);//一秒拍摄15帧
                        colorStream.setVideoMode(colorMode);

                        //打开彩色流
                        colorStream.start();
                        appendMsg("------相机检测------");

                        isCameraOpen = true;
                        emit signalOpen();
                    }
                    else
                    {
                        //appendMsg("打开设备失败,请连接相机，并重新开始！");
                        visualTime->stop();
                        ui.setupdialog->setEnabled(true);
                        QMessageBox::critical(this, "错误提示框", "打开设备失败，请连接相机，并重新开始！\n");
                     
                    }
                    
                }
                else
                {
                    appendMsg("初始化失败");
                }

                
            }
    /*    });
    t.join();*/
    return;
}