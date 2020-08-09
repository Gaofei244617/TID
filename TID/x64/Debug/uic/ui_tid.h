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
    QRadioButton *radioButtonRegion;
    QRadioButton *radioButtonLane;
    QRadioButton *radioButtonDirect;
    QRadioButton *radioButtonLoop;
    QPushButton *clearBtn;
    QPushButton *openBtn;
    QPushButton *viewBtn;

    void setupUi(QMainWindow *TIDClass)
    {
        if (TIDClass->objectName().isEmpty())
            TIDClass->setObjectName(QString::fromUtf8("TIDClass"));
        TIDClass->resize(834, 581);
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
        font.setPointSize(10);
        label->setFont(font);

        horizontalLayout->addWidget(label);

        radioButtonRegion = new QRadioButton(centralWidget);
        radioButtonRegion->setObjectName(QString::fromUtf8("radioButtonRegion"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(radioButtonRegion->sizePolicy().hasHeightForWidth());
        radioButtonRegion->setSizePolicy(sizePolicy);
        QFont font1;
        font1.setFamily(QString::fromUtf8("Consolas"));
        radioButtonRegion->setFont(font1);

        horizontalLayout->addWidget(radioButtonRegion);

        radioButtonLane = new QRadioButton(centralWidget);
        radioButtonLane->setObjectName(QString::fromUtf8("radioButtonLane"));
        sizePolicy.setHeightForWidth(radioButtonLane->sizePolicy().hasHeightForWidth());
        radioButtonLane->setSizePolicy(sizePolicy);
        radioButtonLane->setFont(font1);

        horizontalLayout->addWidget(radioButtonLane);

        radioButtonDirect = new QRadioButton(centralWidget);
        radioButtonDirect->setObjectName(QString::fromUtf8("radioButtonDirect"));
        sizePolicy.setHeightForWidth(radioButtonDirect->sizePolicy().hasHeightForWidth());
        radioButtonDirect->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(radioButtonDirect);

        radioButtonLoop = new QRadioButton(centralWidget);
        radioButtonLoop->setObjectName(QString::fromUtf8("radioButtonLoop"));
        sizePolicy.setHeightForWidth(radioButtonLoop->sizePolicy().hasHeightForWidth());
        radioButtonLoop->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(radioButtonLoop);

        clearBtn = new QPushButton(centralWidget);
        clearBtn->setObjectName(QString::fromUtf8("clearBtn"));
        sizePolicy.setHeightForWidth(clearBtn->sizePolicy().hasHeightForWidth());
        clearBtn->setSizePolicy(sizePolicy);
        clearBtn->setFont(font1);

        horizontalLayout->addWidget(clearBtn);

        openBtn = new QPushButton(centralWidget);
        openBtn->setObjectName(QString::fromUtf8("openBtn"));
        sizePolicy.setHeightForWidth(openBtn->sizePolicy().hasHeightForWidth());
        openBtn->setSizePolicy(sizePolicy);
        openBtn->setFont(font1);

        horizontalLayout->addWidget(openBtn);

        viewBtn = new QPushButton(centralWidget);
        viewBtn->setObjectName(QString::fromUtf8("viewBtn"));
        sizePolicy.setHeightForWidth(viewBtn->sizePolicy().hasHeightForWidth());
        viewBtn->setSizePolicy(sizePolicy);
        QFont font2;
        font2.setFamily(QString::fromUtf8("Consolas"));
        font2.setBold(true);
        font2.setWeight(75);
        viewBtn->setFont(font2);

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
        radioButtonRegion->setText(QCoreApplication::translate("TIDClass", "\345\210\206\346\236\220\345\214\272\345\237\237", nullptr));
        radioButtonLane->setText(QCoreApplication::translate("TIDClass", "\350\275\246\351\201\223", nullptr));
        radioButtonDirect->setText(QCoreApplication::translate("TIDClass", "\346\226\271\345\220\221", nullptr));
        radioButtonLoop->setText(QCoreApplication::translate("TIDClass", "\350\231\232\346\213\237\347\272\277\345\234\210", nullptr));
        clearBtn->setText(QCoreApplication::translate("TIDClass", "Clear", nullptr));
        openBtn->setText(QCoreApplication::translate("TIDClass", "Open", nullptr));
        viewBtn->setText(QCoreApplication::translate("TIDClass", "View Json", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TIDClass: public Ui_TIDClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TID_H
