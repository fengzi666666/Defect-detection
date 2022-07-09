#include "mainwindow.h"

//给PLC写数据s
void MainWindow::writeRequst()
{
    int star_add = ui.portBoxOUT->value();//写入数据的起始地址 
    quint16 value = 1;//要写入的数据
    quint16 number = 1;//写入数据的个数
    if (!modbusDevice)
    {
        appendMsg("Modbus模块建立出错");
        return;
    }
    //quint16 number1 = static_cast<quint16>(number);
    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::Coils, star_add, number);//参数：寄存器类型，数据起始地址，数据大小
    //int a = writeUnit.valueCount();
    //for (uint i = 0; i < writeUnit.valueCount(); i++)//循环写入
    //{
        //int ii = static_cast<int>(i);//将uint类型数值静态转换为int型。
        //QString st = str1.mid(ii, 1);//获取指定位置子串,参数1从哪个开始，参数2一共几位
    writeUnit.setValue(0, value);//将用户输出的数据设置到写容器中
    //quint16 a = writeUnit.value(0);
    if (auto* reply = modbusDevice->sendWriteRequest(writeUnit, 0xFF))//1为服务器的serveAddress
    {
        if (!reply->isFinished())//返回未终止时
        {
            connect(reply, &QModbusReply::finished, this, [=]() {
                if (reply->error() == QModbusDevice::ProtocolError)
                {
                    QString errorM1 = QString::fromLocal8Bit("Write response error: %1 (Mobus exception: 0x%2)").arg(reply->errorString()).arg(reply->rawResult().exceptionCode(), -1, 16);
                    appendMsg(errorM1);
                }
                else if (reply->error() != QModbusDevice::NoError)//错误
                {
                    QString errorM2 = QString::fromLocal8Bit("Write response error: %1 (code: 0x%2)").arg(reply->errorString()).arg(reply->error(), -1, 16);
                    appendMsg(errorM2);
                }
                appendMsg("写入成功，数据为：" + QString::number(value));
                //emit signalRead();
                reply->deleteLater();
                });
        }
        else
        {
            //广播消息 不需要返回响应
            reply->deleteLater();// broadcast replies return immediately
        }
    }
    else
    {
        //QString errorM3 = ("写入错误： ") + modbusDevice->errorString();
        //appendMsg(errorM3);
    }
}