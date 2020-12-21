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
	void clickOnEditBtn();
	void clickOnApplyBtn();

public:
	Ui::ParamView* ui;
	QString video_path;

private:
	QButtonGroup* buttonGroup;
};

