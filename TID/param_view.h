#pragma once

#include <QtWidgets/QWidget>
#include "ui_param_view.h"
#include <QString>
#include <QButtonGroup>
#include "MyQTextEdit.h"

class ParamView : public QWidget
{
	Q_OBJECT

public:
	ParamView(QWidget* parent = Q_NULLPTR);
	void setContent(const QString& str);
	void formatContent(int format);
	void clickOnExportBtn();

public:
	Ui::ParamView* ui;

private:
	QButtonGroup* buttonGroup;
};

