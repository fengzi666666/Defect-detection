#pragma once

#include <QDialog>
#include "ui_setdialog.h"
#include< QFileDialog>
#include <QString>
#include<iostream>
class setDialog : public QDialog
{
	Q_OBJECT
private:
	Ui::setDialog ui;

public:
	setDialog(QWidget *parent = Q_NULLPTR);
	~setDialog();

public slots:
	void on_acceptButton_clicked();
	void on_rejectButton_clicked();
	void on_cameraRadioButton_clicked();
	void on_videoRadioButton_clicked();
	void on_pictureRadioButton_clicked();
	void getMessage(bool& isCamera, bool& isVedio, bool& isImage,bool& isResolvingPower,std::string& vedioStr, std::string& imageStr);
	

};
