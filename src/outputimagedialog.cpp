#include "outputimagedialog.h"

outputImageDialog::outputImageDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
}

outputImageDialog::~outputImageDialog()
{
}
void outputImageDialog::on_acceptButton_clicked() {
	this->accept();
}
void outputImageDialog::on_rejectButton_clicked() {
	reject();
}
void outputImageDialog::on_choseButton_clicked() {
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	//QString fileName = QFileDialog::getOpenFileName(NULL, QStringLiteral("选择视频"), "./images", QStringLiteral("*mp4"));
	ui.imageSaveChose->setText(dir);
}

void outputImageDialog::getMessage(bool& isSaveImage1, bool& isSaveImage2, bool& isSaveImage3, QString& imageSavePath) {
	isSaveImage1 = ui.saveImage_1->isChecked();
	isSaveImage2 = ui.saveImage_2->isChecked();
	isSaveImage3 = ui.saveImage_3->isChecked();
	imageSavePath = ui.imageSaveChose->text();
	
}