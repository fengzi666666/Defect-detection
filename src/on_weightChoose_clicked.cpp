#include "mainwindow.h"


void MainWindow::on_weightChoose_clicked() {

	QString fileName = QFileDialog::getOpenFileName(this, QStringLiteral("Weight"), "./weights1", QStringLiteral("*onnx"));
	ui.weightChose_2->setText(fileName);
	modelPath = fileName.toLocal8Bit();

}