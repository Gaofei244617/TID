#include "param_view.h"
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>

ParamView::ParamView(QWidget* parent)
	: QWidget(parent),
	ui(new Ui::ParamView())
{
	ui->setupUi(this);
    QObject::connect(ui->exportButton, &QPushButton::clicked, this, &ParamView::clickOnExportBtn);
}

void ParamView::setContent(const QString& str)
{
	ui->textEdit->setText(str);
}

void ParamView::clickOnExportBtn()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Json"), tr("param.json"), tr("Json Files (*.json)"));
    if (!fileName.isNull())
    {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream stream(&file);
            stream << ui->textEdit->toPlainText();
            stream.flush();
            file.close();
        }
        else
        {
            QMessageBox::about(this, "", tr("fail to open file"));
        }
    }
    return;
}

