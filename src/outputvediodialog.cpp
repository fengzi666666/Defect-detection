#include "outputvediodialog.h"

outputVedioDialog::outputVedioDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
}

outputVedioDialog::~outputVedioDialog()
{
}
void outputVedioDialog::on_acceptButton_clicked() {
	this->accept();
}
void outputVedioDialog::on_rejectButton_clicked() {
	reject();
}
void outputVedioDialog::on_choseButton_clicked() {
	QString dir = QFileDialog::getSaveFileName(this, "save file", "E:/programs data/QT/Defect detection/detection_data/vedioSave", "(*.mp4);");
	//QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	//QString fileName = QFileDialog::getOpenFileName(NULL, QStringLiteral("选择视频"), "./images", QStringLiteral("*mp4"));
	ui.videoSaveChose->setText(dir);
}

void outputVedioDialog::getMessage(bool& saveVedioLeft, bool& saveVedioRight, QString& videoSaveChose) {

	saveVedioLeft = ui.saveVedioLeft->isChecked();
	saveVedioRight = ui.saveVedioRight->isChecked();
	videoSaveChose = ui.videoSaveChose->text();
}
