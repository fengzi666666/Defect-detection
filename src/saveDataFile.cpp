#include "mainwindow.h"

//保存缺陷图片
void MainWindow::saveDataFile()
{
    std::thread t([=]() {
        //选择一个路径
        QDateTime dateTime(QDateTime::currentDateTime());//构造函数
        QString qStr = dateTime.toString("yyMMddhhmm");
        if (qStr!=timeStr) {
            g_imagecount = 1;
        }
        timeStr = qStr;
        QString filePath = savePath + qStr + "_" + QString::number(g_imagecount++) + ".bmp";
        cv::imwrite(filePath.toStdString(), binaryMat);//将彩色图保存到本地
        //qDebug() << colorMat.rows;
        appendMsg("缺陷数据保存完成,查看:C:/hole_detection/dataset");
        });
    t.join();
}

//QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);