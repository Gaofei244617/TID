/********************************************************************************
** Form generated from reading UI file 'param_view.ui'
**
** Created by: Qt User Interface Compiler version 5.15.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PARAM_VIEW_H
#define UI_PARAM_VIEW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ParamView
{
public:
    QGridLayout *gridLayout;
    QTextBrowser *textBrowser;

    void setupUi(QWidget *ParamView)
    {
        if (ParamView->objectName().isEmpty())
            ParamView->setObjectName(QString::fromUtf8("ParamView"));
        ParamView->resize(400, 300);
        gridLayout = new QGridLayout(ParamView);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(5, 5, 5, 5);
        textBrowser = new QTextBrowser(ParamView);
        textBrowser->setObjectName(QString::fromUtf8("textBrowser"));
        QFont font;
        font.setFamily(QString::fromUtf8("Consolas"));
        textBrowser->setFont(font);

        gridLayout->addWidget(textBrowser, 0, 0, 1, 1);


        retranslateUi(ParamView);

        QMetaObject::connectSlotsByName(ParamView);
    } // setupUi

    void retranslateUi(QWidget *ParamView)
    {
        ParamView->setWindowTitle(QCoreApplication::translate("ParamView", "Json String", nullptr));
        textBrowser->setPlaceholderText(QString());
    } // retranslateUi

};

namespace Ui {
    class ParamView: public Ui_ParamView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PARAM_VIEW_H
