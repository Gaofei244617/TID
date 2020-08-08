/********************************************************************************
** Form generated from reading UI file 'tid.ui'
**
** Created by: Qt User Interface Compiler version 5.15.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TID_H
#define UI_TID_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "MyQGraphicsView.h"

QT_BEGIN_NAMESPACE

class Ui_TIDClass
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    MyQGraphicsView *graphicsView;
    QSlider *slider;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QRadioButton *radioButtonLane;
    QRadioButton *radioButtonRegion;
    QPushButton *openBtn;
    QPushButton *viewBtn;

    void setupUi(QMainWindow *TIDClass)
    {
        if (TIDClass->objectName().isEmpty())
            TIDClass->setObjectName(QString::fromUtf8("TIDClass"));
        TIDClass->resize(630, 467);
        centralWidget = new QWidget(TIDClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        graphicsView = new MyQGraphicsView(centralWidget);
        graphicsView->setObjectName(QString::fromUtf8("graphicsView"));
        graphicsView->setMouseTracking(true);
        graphicsView->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

        verticalLayout->addWidget(graphicsView);

        slider = new QSlider(centralWidget);
        slider->setObjectName(QString::fromUtf8("slider"));
        slider->setMaximum(1000);
        slider->setOrientation(Qt::Horizontal);

        verticalLayout->addWidget(slider);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));
        QFont font;
        font.setFamily(QString::fromUtf8("Consolas"));
        label->setFont(font);

        horizontalLayout->addWidget(label);

        radioButtonLane = new QRadioButton(centralWidget);
        radioButtonLane->setObjectName(QString::fromUtf8("radioButtonLane"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(radioButtonLane->sizePolicy().hasHeightForWidth());
        radioButtonLane->setSizePolicy(sizePolicy);
        radioButtonLane->setFont(font);

        horizontalLayout->addWidget(radioButtonLane);

        radioButtonRegion = new QRadioButton(centralWidget);
        radioButtonRegion->setObjectName(QString::fromUtf8("radioButtonRegion"));
        sizePolicy.setHeightForWidth(radioButtonRegion->sizePolicy().hasHeightForWidth());
        radioButtonRegion->setSizePolicy(sizePolicy);
        radioButtonRegion->setFont(font);

        horizontalLayout->addWidget(radioButtonRegion);

        openBtn = new QPushButton(centralWidget);
        openBtn->setObjectName(QString::fromUtf8("openBtn"));
        sizePolicy.setHeightForWidth(openBtn->sizePolicy().hasHeightForWidth());
        openBtn->setSizePolicy(sizePolicy);
        openBtn->setFont(font);

        horizontalLayout->addWidget(openBtn);

        viewBtn = new QPushButton(centralWidget);
        viewBtn->setObjectName(QString::fromUtf8("viewBtn"));
        sizePolicy.setHeightForWidth(viewBtn->sizePolicy().hasHeightForWidth());
        viewBtn->setSizePolicy(sizePolicy);
        QFont font1;
        font1.setFamily(QString::fromUtf8("Consolas"));
        font1.setBold(true);
        font1.setWeight(75);
        viewBtn->setFont(font1);

        horizontalLayout->addWidget(viewBtn);


        verticalLayout->addLayout(horizontalLayout);

        TIDClass->setCentralWidget(centralWidget);

        retranslateUi(TIDClass);

        QMetaObject::connectSlotsByName(TIDClass);
    } // setupUi

    void retranslateUi(QMainWindow *TIDClass)
    {
        TIDClass->setWindowTitle(QCoreApplication::translate("TIDClass", "TID", nullptr));
        label->setText(QString());
        radioButtonLane->setText(QCoreApplication::translate("TIDClass", "\350\275\246\351\201\223", nullptr));
        radioButtonRegion->setText(QCoreApplication::translate("TIDClass", "\345\210\206\346\236\220\345\214\272\345\237\237", nullptr));
        openBtn->setText(QCoreApplication::translate("TIDClass", "Open", nullptr));
        viewBtn->setText(QCoreApplication::translate("TIDClass", "View Json", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TIDClass: public Ui_TIDClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TID_H
