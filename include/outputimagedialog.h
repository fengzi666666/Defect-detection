#pragma once

#include <QDialog>
#include "ui_outputimagedialog.h"
#include< QFileDialog>
#include <QString>
#include<iostream>
class outputImageDialog : public QDialog
{
	Q_OBJECT

public:
	outputImageDialog(QWidget *parent = Q_NULLPTR);
	~outputImageDialog();

private:
	Ui::outputImageDialog ui;

public slots:
	void on_acceptButton_clicked();
	void on_rejectButton_clicked();
	void on_choseButton_clicked();
	void getMessage(bool& isSaveImage1, bool& isSaveImage2, bool& isSaveImage3, QString& imageSavePath);
};
