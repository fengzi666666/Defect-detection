#include "mainwindow.h"
#include<qdebug.h>
//连接状态发生改变时处理函数（connect or discennect）
void MainWindow::onStateChanged(int state)
{
    //判断Modbus设备连接是否处于连接状态
    if (state == QModbusDevice::UnconnectedState)
    {   
        isConnected = false;
        appendMsg("已断开");
        //qDebug() << "TCP Client连接到Server 未连接";
        ui.connectPLC->setText(tr("连接PLC"));
        
        //ui.startButton->setEnabled(false);
        //ui.closeButton->setEnabled(false);
        
    }
    else if (state == QModbusDevice::ConnectingState)
    {   
        isConnected = false;
        appendMsg("TCP Client正在连接Server");
        //qDebug() << "TCP Client正在连接Server";
    }
    else if (state == QModbusDevice::ConnectedState)
    {   
        isConnected = true;
        appendMsg("TCP Client已经连接到Server");
        //qDebug() << "TCP Client已经连接到Server";
        ui.connectPLC->setText(tr("断开连接"));

        //ui.startButton->setEnabled(true);
        //ui.closeButton->setEnabled(true);
       
    }
    else if (state == QModbusDevice::ClosingState)
    {
        appendMsg("设备已经关闭");
        //qDebug() << "设备已经被关闭";
    }

    //appendMsg(QString::number(modbusDevice->state()));//2表示连接成功
}