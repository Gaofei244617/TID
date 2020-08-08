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
    ui->radioButtonLane->setChecked(true);
    
    // �����ź�/��
    QObject::connect(ui->viewBtn, &QPushButton::clicked, this, &TID::click_on_viewBtn);
    QObject::connect(ui->graphicsView, &MyQGraphicsView::mouseMoveSignal, this, &TID::showPoint);
    QObject::connect(ui->graphicsView, &MyQGraphicsView::dropFile, this, &TID::setWindowTitle);
    QObject::connect(ui->slider, &QSlider::valueChanged, ui->graphicsView, &MyQGraphicsView::onSliderChangeed);
    QObject::connect(buttonGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), ui->graphicsView, &MyQGraphicsView::setRegionMode);
}


void TID::click_on_viewBtn()
{
    paramView->setContent("{\"ImageWidth\": 1920}");
    paramView->show();
}

void TID::showPoint(const QPoint& pt)
{
    ui->label->setText(QString("[%1, %2]").arg(pt.x()).arg(pt.y()));
}

