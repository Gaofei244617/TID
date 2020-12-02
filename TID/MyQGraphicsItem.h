#pragma once

#include <QGraphicsItem>
#include "common.h"

class MyQGraphicsItem :public QGraphicsItem
{
public:
    MyQGraphicsItem(QGraphicsItem* parent = nullptr);
    ~MyQGraphicsItem();
    void updateParam(const QString& mode, const TIDContour& contour, const QVector<QPoint>& pts, const QVector<QPoint>& vecPoint, const QPoint& pt);

protected:
    QRectF boundingRect()const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

private:
    TIDContour m_contour;
    QVector<>
    QVector<QPoint> m_mesureData;
    QVector<QPoint> vecPointCache;
    QPoint ptCache;
    QString mode;
};