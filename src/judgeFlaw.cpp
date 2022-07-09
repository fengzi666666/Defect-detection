#include "mainwindow.h"

//判断缺陷
void MainWindow::judgeFlaw() {
    std::thread t([=]() {
        //判断每一个像素点的灰度值是否在阈值
        int height = binaryMat.rows;
        int width = binaryMat.cols;
        int channels = binaryMat.channels();
        int numberOfBlack = 0;

        for (int i = 0; i < height; i++)
        {
            uchar* data = binaryMat.ptr<uchar>(i);//data指向图像中第i+1行的第一个元素
            for (int j = 0; j < width; j++)
            {
                if (data[j] == 0) //二值图中黑色的部分
                {
                    numberOfBlack++;//黑色的元素的个数
                }
            }
        }
        if (numberOfBlack > ui.numBox->value())
        {
            appendMsg("有漏洞缺陷");
            //改变指示灯颜色为红色
            this->isHaveFlaw = true;
            //g_imagecount++;
            //保存缺陷数据
            emit signalFlaw();
            //发送信号
            if (isConnected == true)
            {
                //emit signalRead();
                emit signalWrite();
            }
        }
        });
    t.join();
}