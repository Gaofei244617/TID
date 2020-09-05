#include "param_view.h"
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include "common.h"

ParamView::ParamView(QWidget* parent)
	: QWidget(parent),
	ui(new Ui::ParamView())
{
	ui->setupUi(this);

    buttonGroup = new QButtonGroup(this);
    buttonGroup->setExclusive(true);
    buttonGroup->addButton(ui->fmtBtn1, 0);
    buttonGroup->addButton(ui->fmtBtn2, 1);
    buttonGroup->addButton(ui->fmtBtn3, 2);
    ui->fmtBtn1->setChecked(true);

    QObject::connect(ui->exportButton, &QPushButton::clicked, this, &ParamView::clickOnExportBtn);
    QObject::connect(ui->editButton, &QPushButton::clicked, this, &ParamView::clickOnEditBtn);
    QObject::connect(ui->applyButton, &QPushButton::clicked, this, &ParamView::clickOnApplyBtn);
    QObject::connect(buttonGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), this, &ParamView::formatContent);
}

void ParamView::setContent(const QString& str)
{
	ui->textEdit->setText(str);
}

void ParamView::formatContent(int format)
{
    QString json = ui->textEdit->toPlainText();
    rapidjson::Document doc;
    doc.Parse(json.toStdString().c_str());
    if (doc.HasParseError())
    {
        QMessageBox::about(this, "", tr("fail to format json"));
        return;
    }

    // ¸ñÊ½»¯json×Ö·û´®
    switch (format)
    {
    case 0:
        ui->textEdit->setText(JsonToPrettyString(doc));
        break;
    case 1:
        ui->textEdit->setText(JsonToPrettyString2(doc));
        break;
    case 2:
        ui->textEdit->setText(JsonToString(doc));
        break;
    }
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

void ParamView::clickOnEditBtn()
{
    ui->textEdit->setReadOnly(false);
}

void ParamView::clickOnApplyBtn()
{
    QString txt(ui->textEdit->toPlainText());
    emit ui->textEdit->inputFileSignal(txt);
    ui->textEdit->setReadOnly(true);
}
