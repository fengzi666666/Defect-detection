#include "mainwindow.h"

//打开相机
void MainWindow::on_startButton_clicked()//打开相机
{
    if (this->isConnected == false)
    {
        QMessageBox::critical(this, "错误提示框", "请先连接PLC\n");
    }
    else
    {
        //开始检测不可切换GPU CPU
        if (ui.GPU->isChecked()) {
            ui.CPU->setCheckable(false);
        }
        if (ui.CPU->isChecked()) {
            ui.GPU->setCheckable(false);
        }
        
        

        if (isCamera) {
            
                this->openCamera();
                

        }
        else if (isVideo) {
            if (!isCameraOpen)
            {
                vid_capture= VideoCapture(vedioPath);//创建一个视频捕捉对象
                QString fps = QString::number(vid_capture.get(5),'f',0);//源视频帧数
                QString frame_count = QString::number(vid_capture.get(7),'f', 0);//统计视频帧数
                appendMsg("------视频检测------");
                appendMsg("帧数 :" + fps+"   "+"总帧数 :" + frame_count);
                isCameraOpen = true;
                emit signalOpen();
                //设置开始按钮不可点击
                ui.startButton->setEnabled(false);
                ui.closeButton->setEnabled(true);
                ui.setupdialog->setEnabled(false);
            }
        }
        else if (isImage) {
            appendMsg("------图片检测------");
            isCameraOpen = true;
            emit signalOpen();
            //设置开始按钮不可点击
            ui.startButton->setEnabled(false);
            ui.closeButton->setEnabled(true);
            ui.setupdialog->setEnabled(false);
        }
        ////设置开始按钮不可点击
        //ui.startButton->setEnabled(false);
        //ui.closeButton->setEnabled(true);
        
        
    }
}