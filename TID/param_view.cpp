#include "param_view.h"

ParamView::ParamView(QWidget* parent)
	: QWidget(parent),
	ui(new Ui::ParamView())
{
	ui->setupUi(this);
}

void ParamView::setContent(const QString& str)
{
	ui->textBrowser->setText(str);
}
