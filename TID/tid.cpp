#include "tid.h"
#include <QMessageBox>
#include "MyQGraphicsView.h"

TID::TID(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::TIDClass()),
    paramView(new ParamView())
{
    ui->setupUi(this);
    // �����ʮ�ֹ��
    ui->graphicsView->setCursor(Qt::CrossCursor);
    // ���������٣���ʹû�а��°�ť��Ҳ���������ƶ��¼���
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
    
    // �����ź�/��
    QObject::connect(ui->viewBtn, &QPushButton::clicked, this, &TID::clickOnViewBtn);
    QObject::connect(ui->graphicsView, &MyQGraphicsView::mouseMoveSignal, this, &TID::showPoint);
    QObject::connect(ui->graphicsView, &MyQGraphicsView::dropFile, this, &TID::setWindowTitle);
    QObject::connect(ui->slider, &QSlider::valueChanged, ui->graphicsView, &MyQGraphicsView::onSliderChangeed);
    QObject::connect(buttonGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), ui->graphicsView, &MyQGraphicsView::setRegionMode);
    QObject::connect(ui->graphicsView, &MyQGraphicsView::updateJson, paramView, &ParamView::setContent);
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

