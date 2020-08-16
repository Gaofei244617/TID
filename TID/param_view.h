#pragma once

#include <QtWidgets/QWidget>
#include "ui_param_view.h"
#include <QString>

class ParamView : public QWidget
{
	Q_OBJECT

public:
	ParamView(QWidget* parent = Q_NULLPTR);
	void setContent(const QString& str);
	void clickOnExportBtn();

private:
	Ui::ParamView* ui;
};

