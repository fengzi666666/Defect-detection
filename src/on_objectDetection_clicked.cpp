#include "mainwindow.h"

void MainWindow::on_objectDetection_clicked()//目标检测被点击
{
	ui.GPU->setAutoExclusive(false);
	ui.CPU->setAutoExclusive(false);
	ui.GPU->setCheckable(true);
	ui.CPU->setCheckable(true);
	if (visualTime->isActive()) {
		if (initGPU) {
			ui.GPU->setChecked(true);
			ui.CPU->setChecked(false);
			ui.CPU->setCheckable(false);
		}
		else {
			ui.GPU->setChecked(false);
			ui.CPU->setChecked(true);
			ui.GPU->setCheckable(false);
		}
	}
	
	ui.GPU->setAutoExclusive(true);
	ui.CPU->setAutoExclusive(true);
}