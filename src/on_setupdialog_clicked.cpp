#include "mainwindow.h"

void MainWindow::on_setupdialog_clicked() {

	//setDialog* s = new setDialog;
	////setDialog s;
	int nResult = -1;
	pDlg->setWindowTitle(tr("检测对象设置"));
	nResult=pDlg->exec();
	if (nResult == setDialog::Accepted) {//确认
		pDlg->getMessage(isCamera, isVideo, isImage,isResolvingPower,vedioPath, imagePath);
	}
}
