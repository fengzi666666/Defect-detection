#include "mainwindow.h"

//改变指示灯的状态
void MainWindow::changeState()
{
    QString lightPath = NULL;
    //有缺陷提示
    if (this->isHaveFlaw == true)
    {
        lightPath = ":/res/state_red_48.png";
    }
    //处理过缺陷恢复原来状态
    if (this->isHandled == true)
    {
        lightPath = ":/res/state_green_48.png";
    }
    QPixmap light = QPixmap(lightPath);
    ui.lightlabel->setPixmap(light);
}