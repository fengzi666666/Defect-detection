#include "mainwindow.h"

//读取内容
bool MainWindow::readCoils()
{

    int start_add = ui.portBoxIN->value();//待读取的线圈的地址
    quint16 numbers = 10;//读一位

    //QModbusDataUnit::Coils 从地址0开始读取10个线圈值
    QModbusDataUnit data(QModbusDataUnit::Coils, start_add, numbers);

    auto reply = modbusDevice->sendReadRequest(data, 0x1);
    if (nullptr == reply)
    {
       // appendMsg("发送请求数据失败: " + modbusDevice->errorString());
        //qDebug() << "发送请求数据失败: " << modbusDevice->errorString();
        return false;
    }
    else
    {
        if (!reply->isFinished())
        {
            connect(reply, &QModbusReply::finished, this, &MainWindow::readReady_coils);
        }
        else
        {
            //broadcast replies return immediately
            delete reply;
            return false;
        }
    }

    //int start_add = ui.portBoxIN->value();//待读取的线圈的地址
    //quint16 numbers = 1;//读一位
    ///*if (!modbusDevice->state() == QModbusDevice::ConnectedState)
    //{
    //    return false;
    //}*/

    //QModbusDataUnit readUnit(QModbusDataUnit::Coils, start_add, numbers);
    //if (QModbusReply* reply = modbusDevice->sendReadRequest(readUnit, 0xFF))
    //{
    //    //appendMsg("serverAddress: " + QString::number(reply->serverAddress()));
    //    //bool a = reply->isFinished();
    //    if (!reply->isFinished())//未返回结束
    //    {
    //        connect(reply, &QModbusReply::finished, this, &MainWindow::readReady_coils);
    //        return true;
    //    }
    //    else
    //    {
    //        appendMsg("应答结束");
    //        delete reply;
    //        return false;
    //    }
    //}
    //else
    //{
    //    appendMsg("发送读请求数据失败");
    //    return false;
    //}
    //return false;
}