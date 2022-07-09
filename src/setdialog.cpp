#include "setdialog.h"

setDialog::setDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	//connect(ui.acceptButton, SIGNAL(clicked()), this, SLOT(accept()));
	//connect(ui.rejectButton, SIGNAL(clicked()), this, SLOT(reject()));
	
}

setDialog::~setDialog()
{
}


void setDialog::on_acceptButton_clicked() {

	//std::cout << "666";

	this->accept();
}



void setDialog::on_rejectButton_clicked() {
	reject();
}

void setDialog::on_cameraRadioButton_clicked() {
	ui.radioButton640->setCheckable(true);
	ui.radioButto1280->setCheckable(true);
	ui.radioButton640->setChecked(true);
}

void setDialog::on_videoRadioButton_clicked() {
	ui.radioButton640->setCheckable(false);
	ui.radioButto1280->setCheckable(false);
	QString fileName = QFileDialog::getOpenFileName(NULL, QStringLiteral("选择视频"), "./images", QStringLiteral("*mp4"));
	ui.videoChose->setText(fileName);

}
void setDialog::on_pictureRadioButton_clicked() {
	ui.radioButton640->setCheckable(false);
	ui.radioButto1280->setCheckable(false);
	QString fileName = QFileDialog::getOpenFileName(this, QStringLiteral("选择图片"), "./images", QStringLiteral("*jpg *bmp"));
	ui.pictureChose->setText(fileName);

}


void setDialog::getMessage(bool& isCamera, bool& isVedio,bool& isImage, bool& isResolvingPower,std::string &vedioStr,std::string & imageStr) {
	isCamera = ui.cameraRadioButton->isChecked();
	isVedio = ui.videoRadioButton->isChecked();
	isImage = ui.pictureRadioButton->isChecked();
	vedioStr= ui.videoChose->text().toLocal8Bit();
	imageStr= ui.pictureChose->text().toLocal8Bit();
	isResolvingPower = ui.radioButton640->isChecked();
}