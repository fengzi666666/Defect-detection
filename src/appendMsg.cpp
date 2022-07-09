#include "mainwindow.h"
//消息框
void MainWindow::appendMsg(QString str)
{
    QDateTime dateTime(QDateTime::currentDateTime());//获取当前时间
    QString qStr = dateTime.toString("yy/MM/dd hh:mm:ss");//转字符串
    ui.textEdit->append(qStr + ": " + str);
}