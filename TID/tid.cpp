#include "tid.h"
#include <QMessageBox>
#include "MyQGraphicsView.h"
#include <QFileDialog>
#include "MyQTextEdit.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

TID::TID(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::TIDClass()),
    paramView(new ParamView())
{
    ui->setupUi(this);
    // 鼠标变成十字光标
    ui->graphicsView->setCursor(Qt::CrossCursor);
    // 启用鼠标跟踪，即使没有按下按钮，也会接收鼠标移动事件。
    ui->graphicsView->setMouseTracking(true);
    ui->graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
    
    ui->slider->setMaximum(1000);
    ui->slider->setSingleStep(1);

    buttonGroup = new QButtonGroup(this);
    buttonGroup->setExclusive(true);
    buttonGroup->addButton(ui->radioButtonLane, 0);
    buttonGroup->addButton(ui->radioButtonRegion, 1);
    buttonGroup->addButton(ui->radioButtonDirect, 2);
    buttonGroup->addButton(ui->radioButtonLoop, 3);
    ui->radioButtonLane->setChecked(true);
    
    // 连接信号/槽
    QObject::connect(ui->viewBtn, &QPushButton::clicked, this, &TID::clickOnViewBtn);
    QObject::connect(ui->graphicsView, &MyQGraphicsView::mouseMoveSignal, this, &TID::showPoint);
    QObject::connect(ui->graphicsView, &MyQGraphicsView::openFileSignal, this, &TID::onOpenVideo);
    QObject::connect(ui->slider, &QSlider::valueChanged, ui->graphicsView, &MyQGraphicsView::onSliderChangeed);
    QObject::connect(buttonGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), ui->graphicsView, &MyQGraphicsView::setRegionMode);
    QObject::connect(ui->graphicsView, &MyQGraphicsView::updateJsonSignal, paramView, &ParamView::setContent);
    QObject::connect(ui->clearBtn, &QPushButton::clicked, ui->graphicsView, &MyQGraphicsView::clearContour);
    QObject::connect(ui->openBtn, &QPushButton::clicked, this, &TID::clickOnOpenFile);
    QObject::connect(paramView->ui->textEdit, &MyQTextEdit::inputFileSignal, ui->graphicsView, &MyQGraphicsView::setContour);
}

void TID::onOpenVideo(const QString& info)
{
    QJsonDocument doc = QJsonDocument::fromJson(info.toUtf8());
    QJsonObject obj = doc.object();
    QString name = obj.take("Name").toString();
    QString frameInfo = QString::fromLocal8Bit("帧率: [%1]   帧数: [%2]")
        .arg(obj.take("FPS").toDouble())
        .arg(obj.take("FrameCount").toDouble());

    this -> setWindowTitle(name);
    ui->label2->setText(frameInfo);
}

// ViewJson
void TID::clickOnViewBtn()
{
    paramView->show();
}

// label show point
void TID::showPoint(const QPoint& pt)
{
    ui->label->setText(QString("[%1, %2]").arg(pt.x()).arg(pt.y()));
}

void TID::clickOnOpenFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", "", 0);
    ui->graphicsView->actionOnOpenFile(fileName);
}
