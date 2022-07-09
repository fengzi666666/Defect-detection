#include "mainwindow.h"

void MainWindow::on_imageDetection_clicked()//目标检测被点击
{	
	//std::thread t([=]()
	//{ 
		ui.GPU->setAutoExclusive(false);
		ui.CPU->setAutoExclusive(false);
		ui.GPU->setChecked(false);
		ui.CPU->setChecked(false);
		ui.GPU->setCheckable(false);
		ui.CPU->setCheckable(false);
		ui.GPU->setAutoExclusive(true);
		ui.CPU->setAutoExclusive(true);
	/*});
	 t.join();*/
}