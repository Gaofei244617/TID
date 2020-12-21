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
    QObject::connect(buttonGroup, &QButtonGroup::idClicked, this, &ParamView::formatContent);
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

    // 格式化json字符串
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
    QFileInfo fi(this->video_path);
    auto dir = fi.path();
    auto baseName = fi.completeBaseName();
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Json"), dir + "/" + baseName + ".json", tr("Json Files (*.json)"));
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
    // 设置为可编辑状态
    if (ui->textEdit->isReadOnly())
    {
        ui->textEdit->setReadOnly(false);
        ui->editButton->setText(QString::fromLocal8Bit("取消编辑"));
        QPalette palette = ui->textEdit->palette();
        palette.setColor(QPalette::Base, Qt::black);
        palette.setColor(QPalette::Text, Qt::green);
        ui->textEdit->setPalette(palette);
    }
    // 设置为不可编辑状态
    else
    {
        ui->textEdit->setReadOnly(true);
        ui->editButton->setText(QString::fromLocal8Bit("编辑"));
        QPalette palette = ui->textEdit->palette();
        palette.setColor(QPalette::Base, Qt::white);
        palette.setColor(QPalette::Text, Qt::black);
        ui->textEdit->setPalette(palette);
    }
}

void ParamView::clickOnApplyBtn()
{
    QString txt(ui->textEdit->toPlainText());
    emit ui->textEdit->inputFileSignal(txt);
}
