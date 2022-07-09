#include "mainwindow.h"
void  MainWindow::on_imageSaveSet_clicked() {
	int nResult = -1;
	pDlg1->setWindowTitle(tr("图片保存设置"));
	nResult = pDlg1->exec();
	if (nResult == setDialog::Accepted) {//确认
		pDlg1->getMessage(isSaveImage1, isSaveImage2, isSaveImage3, imageSavePath);
	}
}
void  MainWindow::on_vedioSaveSet_clicked() {
	int nResult = -1;
	pDlg2->setWindowTitle(tr("视频保存设置"));
	nResult = pDlg2->exec();
	if (nResult == setDialog::Accepted) {//确认
		pDlg2->getMessage(isSaveVedioLeft, isSaveVedioRight,videoSavePath);
	}
}
void  MainWindow::on_imageSave_clicked() {
    //std::thread t([=]() {
        if (!visualTime->isActive()) {//如果没有开始检测则退出
            QMessageBox::critical(this, "错误提示框", "请开始检测之后再保存！");
            return;
       }
        //选择一个路径
        QDateTime dateTime(QDateTime::currentDateTime());//构造函数
        QString qStr = dateTime.toString("yyMMddhhmm");
        if (qStr != timeStr) {
            g_imagecount = 1;
        }
        timeStr = qStr;
        if (ui.objectDetection->isChecked()) {
            if (isSaveImage2) {
                QString filePath = imageSavePath+"/" + qStr + "_" + QString::number(g_imagecount) + "(raw)" + ".bmp";
                cv::imwrite(filePath.toStdString(), Image1);//将彩色图保存到本地
            }
            if (isSaveImage3) {
                QString filePath = imageSavePath+"/" + qStr + "_" + QString::number(g_imagecount) + "(detect)" + ".bmp";
                cv::imwrite(filePath.toStdString(), Image2);
            }
            g_imagecount++;
        }
        else if (ui.imageDetection->isChecked()) {
            if (isSaveImage1) {
                QString filePath = imageSavePath + "/" + qStr + "_" + QString::number(g_imagecount) + "(raw)" + ".bmp";
                cv::imwrite(filePath.toStdString(), Image1);
            }
            if (isSaveImage2) {
                QString filePath = imageSavePath + "/" + qStr + "_" + QString::number(g_imagecount) + "(canny)" + ".bmp";
                cv::imwrite(filePath.toStdString(), Image2);
            }
            if (isSaveImage3) {
                QString filePath = imageSavePath + "/" + qStr + "_" + QString::number(g_imagecount) + "(binary)" + ".bmp";
                cv::imwrite(filePath.toStdString(), Image3);
            }
            g_imagecount++;
        }

        appendMsg("缺陷数据保存完成,查看:" + imageSavePath);
       /* });
    t.join();*/
}
void  MainWindow::on_vedioSave_clicked() {
    //std::thread t([=]() {
    if (!visualTime->isActive()) {//如果没有开始检测则退出
        QMessageBox::critical(this, "错误提示框", "请开始检测之后再录制视频！");
        return;
    }
    if (isSaveVedio==false) {
        if (videoSavePath.isEmpty()) {
            QMessageBox::critical(this, "错误提示框", "请设置视频保存路径\n");
        }
        else {
            isSaveVedio = true;
            
            /*cv::Size S = cv::Size((int)vid_capture.get(CAP_PROP_FRAME_WIDTH),
                (int)vid_capture.get(CAP_PROP_FRAME_HEIGHT));*/
            cv::Size S;
            if (isResolvingPower) {
                S = cv::Size(640, 480);
            }
            else {
                S = cv::Size(1280, 960);
            }
           
            //打开视频路径，设置基本信息 open函数中你参数跟上面给出的VideoWriter函数是一样的
            string str = videoSavePath.toLocal8Bit();
            outputVideo.open(str, VideoWriter::fourcc('H', '2', '6', '4'), 25, S, true);

           

            ui.vedioSave->setText("结束录制");
        }
    }
    else {
        outputVideo.release();
        ui.vedioSave->setText("视频录制");
        isSaveVedio = false;
    }
        
     //});
     //  t.join();
}