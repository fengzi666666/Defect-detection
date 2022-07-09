#include "mainwindow.h"
//连接硬件PLC
void MainWindow::on_connectPLC_clicked()
{
    QString IP = ui.IP->text();//获取IP地址
    QString Port1 = ui.Port->text();//获取端口
    int Port = Port1.toInt();
    //std::thread t([=]() {
        if (!modbusDevice)
        {
            appendMsg("Modbus模块建立出错");
            return;
        }

        if (modbusDevice->state() != QModbusDevice::ConnectedState)
        {
            appendMsg("开始连接");
            modbusDevice->setConnectionParameter(QModbusDevice::NetworkAddressParameter, IP);
            modbusDevice->setConnectionParameter(QModbusDevice::NetworkPortParameter, Port);

            //设置超时时间
            modbusDevice->setTimeout(50); //0.05秒
            //设置失败重试次数
            modbusDevice->setNumberOfRetries(3);
            if (!modbusDevice->connectDevice())////连接到服务端
            {
                appendMsg("设备连接失败");
                //qDebug() << "modbusDevice->connectDevice failed";
            }
            else
            {
                //appendMsg("设置连接成功");
            }
        }
        else
        {
            //断开连接
            isConnected = false;
            modbusDevice->disconnectDevice();
            ui.connectPLC->setText(tr("连接PLC"));

            //ui.startButton->setEnabled(false);
            //ui.closeButton->setEnabled(false);
            
        }
        //appendMsg(QString::number(modbusDevice->state()));//2表示连接成功



        //if (!myClient)//当没有创建连接对象时
        //{
        //    appendMsg("Modbus模块建立出错");
        //    return;
        //}
        //if (myClient->state() == QModbusDevice::UnconnectedState)//没有连接设备时进行连接
        //{
        //    appendMsg("开始连接");
        //    //const QUrl url = QUrl::fromUserInput("192.168.1.1:502"); //;//获取IP和端口号
        //    myClient->setConnectionParameter(QModbusDevice::NetworkAddressParameter, IP);
        //    myClient->setConnectionParameter(QModbusDevice::NetworkPortParameter, Port);

        //    myClient->setTimeout(500);//超时时间内没有相应，则设置TimeoutError
        //    myClient->setNumberOfRetries(3);//请求失败前客户端执行重试的次数
        //    if (!myClient->connectDevice())//连接设备
        //    {
        //        appendMsg("设置连接失败");
        //    }
        //    else
        //    {
        //        appendMsg("设置连接成功");
        //    }
        //}
        //if (myClient->state() == QModbusDevice::ConnectedState)
        //{
        //    isConnected = false;
        //    myClient->disconnectDevice();//断开连接
        //}
        //appendMsg(QString::number(myClient->state()));//2表示连接成功
    /*    });
    t.join();*/
}