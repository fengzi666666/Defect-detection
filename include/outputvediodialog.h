#pragma once

#include <QDialog>
#include "ui_outputvediodialog.h"
#include< QFileDialog>
#include <QString>
#include<iostream>
class outputVedioDialog : public QDialog
{
	Q_OBJECT

public:
	outputVedioDialog(QWidget *parent = Q_NULLPTR);
	~outputVedioDialog();

private:
	Ui::outputVedioDialog ui;

public slots:
	void on_acceptButton_clicked();
	void on_rejectButton_clicked();
	void on_choseButton_clicked();
	void getMessage(bool& saveVedioLeft, bool& saveVedioRight, QString& videoSaveChose);
};
