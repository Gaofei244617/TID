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
#include <QCloseEvent> 

class TID : public QMainWindow
{
    Q_OBJECT

public:
    TID(QWidget *parent = Q_NULLPTR);

private:
    Ui::TIDClass* ui;
    ParamView paramView;
    QButtonGroup* buttonGroup;

protected:
    void onOpenVideo(const QString& info);
    void clickOnViewBtn();
    void showPoint(const QPoint& pt, const QPoint& pt2);
    void clickOnOpenFile();
    void closeEvent(QCloseEvent* event);  // 关闭窗口
    void TID::clickOnExportPicBtn();      // 导出图片
};
