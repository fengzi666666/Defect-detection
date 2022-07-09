#include "mainwindow.h"


void MainWindow::on_labelChoose_clicked() {
	QString fileName = QFileDialog::getOpenFileName(this, QStringLiteral("Label"), "./", QStringLiteral("*names"));
	ui.labelChose_2->setText(fileName);
	classNamesPath = fileName.toLocal8Bit();
	classNames = utilss::loadNames(classNamesPath);
}


