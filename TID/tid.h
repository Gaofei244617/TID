#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_tid.h"
#include "param_view.h"
#include <QPushButton>
#include <QLabel>
#include <QButtonGroup>
#include "MyQGraphicsScene.h"
#include <QPoint>
#include <QString>

class TID : public QMainWindow
{
    Q_OBJECT

public:
    TID(QWidget *parent = Q_NULLPTR);

private:
    Ui::TIDClass* ui;
    ParamView* paramView;
    QButtonGroup* buttonGroup;

protected:
    void clickOnViewBtn();    
    void showPoint(const QPoint& pt);
    void clickOnOpenFile();
};
