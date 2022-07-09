#include "mainwindow.h"
//关闭相机函数
void MainWindow::closeCamera()//关闭相机函数
{
    if (isCameraOpen)
    {
        delete[] pStreams;

        //停止彩色流
        colorStream.stop();

        //销毁彩色流
        colorStream.destroy();

        //关闭设备
        device.close();

        //释放OpenNI资源
        OpenNI::shutdown();//关闭所有驱动并且正确地清除所有

        appendMsg("------相机关闭------");

        isCameraOpen = false;
        //设置开始按钮不可点击
        ui.startButton->setEnabled(true);
        ui.closeButton->setEnabled(false);
        
    }
    return;
}