#include "mainwindow.h"

//接收到读取请求后执行的槽函数
void MainWindow::readReady_coils()
{
    //QModbusReply这个类存储了来自client的数据,sender()返回发送信号的对象的指针
    QModbusReply* reply = qobject_cast<QModbusReply*>(sender());
    if (!reply)
    {
        return;
    }
    //判断是否出错
    if (reply->error() == QModbusDevice::NoError)
    {
        //处理成功返回的数据
        const QModbusDataUnit unit = reply->result();
        qDebug() << "读取成功，数据为：" << unit.values();
        quint16 res = unit.value(0);  //读取第一位的数据
        appendMsg("读取成功，数据为：" + QString::number(res));


        //if (res == 1)
        //{
        //    appendMsg("读取成功" + QString::number(res));
        //    //设置指示灯状态为绿色
        //    this->isHandled = true;
        //    emit this->signalHandled();
        //}
    }
    else if (reply->error() == QModbusDevice::ProtocolError)//协议错误
    {
        //QString errorR1 = "读取响应协议错误:" + reply->errorString();
        //QString errorR1 =QString::fromLocal8Bit("Read response error: %1 (Mobus exception: 0x%2)").arg(reply->errorString()).arg(reply->rawResult().exceptionCode(), -1, 16);
        //appendMsg(errorR1);
    }
    else
    {
        //QString errorM2 = "读取响应错误:" + reply->errorString();
        //QString errorM2 = QString::fromLocal8Bit("Read response error: %1 (code: 0x%2)").arg(reply->errorString()).arg(reply->error(), -1, 16);
        //appendMsg(errorM2);
    }
    //删除reply
    reply->deleteLater();
}