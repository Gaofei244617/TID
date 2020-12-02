#include "tid.h"
#include <QMessageBox>
#include "MyQGraphicsView.h"
#include <QFileDialog>
#include "MyQTextEdit.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

TID::TID(QWidget* parent)
    : QMainWindow(parent),
    ui(new Ui::TIDClass())
{
    ui->setupUi(this);
    // �����ʮ�ֹ��
    ui->graphicsView->setCursor(Qt::CrossCursor);
    // ���������٣���ʹû�а��°�ť��Ҳ���������ƶ��¼���
    ui->graphicsView->setMouseTracking(true);
    ui->graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
    
    this->resize(QSize(850, 700));

    buttonGroup = new QButtonGroup(this);
    buttonGroup->setExclusive(true);
    buttonGroup->addButton(ui->radioButtonLane, 0);
    buttonGroup->addButton(ui->radioButtonEmLane, 1);
    buttonGroup->addButton(ui->radioButtonRegion, 2);
    buttonGroup->addButton(ui->radioButtonDirect, 3);
    buttonGroup->addButton(ui->radioButtonLoop, 4);
    buttonGroup->addButton(ui->radioButtonMesure, 5);

    ui->radioButtonLane->setChecked(true);
    
    // �����ź�/��
    QObject::connect(ui->viewBtn, &QPushButton::clicked, this, &TID::clickOnViewBtn);
    QObject::connect(ui->graphicsView, &MyQGraphicsView::mouseMoveSignal, this, &TID::showPoint);
    QObject::connect(ui->graphicsView, &MyQGraphicsView::openFileSignal, this, &TID::onOpenVideo);
    QObject::connect(ui->slider, &QSlider::valueChanged, ui->graphicsView, &MyQGraphicsView::onSliderChangeed);
    QObject::connect(buttonGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), ui->graphicsView, &MyQGraphicsView::setRegionMode);
    QObject::connect(ui->graphicsView, &MyQGraphicsView::updateJsonSignal, &paramView, &ParamView::setContent);
    QObject::connect(ui->clearBtn, &QPushButton::clicked, ui->graphicsView, &MyQGraphicsView::clearContour);
    QObject::connect(ui->openBtn, &QPushButton::clicked, this, &TID::clickOnOpenFile);
    QObject::connect(paramView.ui->textEdit, &MyQTextEdit::inputFileSignal, ui->graphicsView, &MyQGraphicsView::setContour);
    QObject::connect(ui->exportPicBtn, &QPushButton::clicked, this, &TID::clickOnExportPicBtn);
}

void TID::onOpenVideo(const QString& info)
{
    QJsonDocument doc = QJsonDocument::fromJson(info.toUtf8());
    QJsonObject obj = doc.object();
    QString name = obj.take("Name").toString();
    QString fileType = obj.take("FileType").toString();
   
    QString frameInfo;
    if (fileType == "Video")
    {
        frameInfo = QString::fromLocal8Bit("֡��: [%1]   ֡��: [%2]   ʱ��: [%3 min]   �ֱ���: [%4*%5]")
            .arg(obj.take("FPS").toDouble())
            .arg(obj.take("FrameCount").toDouble())
            .arg(obj.take("Time").toDouble())
            .arg(obj.take("Width").toInt())
            .arg(obj.take("Height").toInt());
    }
    else if (fileType == "Image")
    {
        frameInfo = QString::fromLocal8Bit("�ֱ���: [%1 * %2]")
            .arg(obj.take("Width").toInt())
            .arg(obj.take("Height").toInt());
    }

    this->setWindowTitle(name);
    paramView.setWindowTitle(name);
    ui->label2->setText(frameInfo);
    ui->slider->setValue(0);
}

// ViewJson
void TID::clickOnViewBtn()
{
    paramView.setWindowTitle(this->windowTitle());
    paramView.show();
}

// label show point
void TID::showPoint(const QPoint& pt, const QPoint& pt2)
{
    ui->label->setText(QString("[%1, %2]  Pixel:[%3, %4]").arg(pt.x()).arg(pt.y()).arg(pt2.x()).arg(pt2.y()));
}

void TID::clickOnOpenFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", "", 0);
    ui->graphicsView->actionOnOpenFile(fileName);
}

// �رմ���
void TID::closeEvent(QCloseEvent* event)
{
    paramView.close();
}

// ����ͼƬ
void TID::clickOnExportPicBtn()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image"), tr("image.jpg"), tr("Image Files (*.jpg)"));
    if (!fileName.isNull())
    {
        cv::imwrite(fileName.toStdString(), ui->graphicsView->getCurrentFrame());
    }
    return;
}


