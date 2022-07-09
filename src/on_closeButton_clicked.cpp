#include "mainwindow.h"

//关闭相机
void MainWindow::on_closeButton_clicked()//关闭相机
{
    if (this->isConnected == false)
    {
        QMessageBox::critical(this, "错误提示框", "请先进行硬件连接\n");
    }
    else
    {   
        appendMsg("------检测结束------");
        //恢复切换切换GPU CPU
        ui.GPU->setCheckable(true);
        ui.CPU->setCheckable(true);

        visualTime->stop();
        if(isCamera)this->closeCamera();
        else if (isVideo) {
            this->vid_capture.release();
            isCameraOpen = false;
            //设置开始按钮不可点击
            ui.startButton->setEnabled(true);
            ui.closeButton->setEnabled(false);
        }
        else if (isImage) {
            isCameraOpen = false;
            //设置开始按钮不可点击
            ui.startButton->setEnabled(true);
            ui.closeButton->setEnabled(false);
        }
            
        ui.setupdialog->setEnabled(true);
        //设置开始按钮不可点击
       /* ui.startButton->setEnabled(true);
        ui.closeButton->setEnabled(false);*/
    }
}
