#include "MyQGraphicsItem.h"
#include <QPainter>
#include <QPen>
#include <QRectF>
#include <QGraphicsScene>
#include <QGraphicsView>

MyQGraphicsItem::MyQGraphicsItem(QGraphicsItem* parent) :
    QGraphicsItem(parent)
{
    ptCache = QPoint(0, 0);
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
}

MyQGraphicsItem::~MyQGraphicsItem(){}

void MyQGraphicsItem::updateParam(const QString& mode, const TIDContour& contour, const QVector<QPoint>& vecPoint, const QPoint& pt)
{
    this->mode = mode;
    this->m_contour = contour;
    this->vecPointCache = vecPoint;
    this->ptCache = pt;
}

QRectF MyQGraphicsItem::boundingRect() const
{
    auto it = this->scene();
    if (it != NULL)
    {
        return it->sceneRect();
    }
    return QRectF(0, 0, 1920, 1080);
}

void MyQGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    //static int c = 10;
    //c += 10;
    Q_UNUSED(option);
    Q_UNUSED(widget);
    //painter->setPen(QPen(Qt::blue, 1));
    //painter->drawRect(10, 10, 20, 20);
    //painter->setPen(QPen(Qt::red, 2));
    //painter->drawLine(0 + c, 0, 30 + c, 20 + c);

    if (m_contour.lanes.size() > 0 || m_contour.regions.size() > 0 || vecPointCache.size() > 0)
    {
        auto size = this->scene()->views().at(0)->size();
        // 分析区域
        painter->setPen(QPen(Qt::yellow, 2));
        for (const auto& it : m_contour.regions)
        {
            const TIDRegion& region = it.second;
            QPolygon polygon(region.pts.size());
            for (int i = 0; i < region.pts.size(); i++)
            {
                polygon[i] = toPixelPoint(region.pts[i], size);
            }
            painter->drawPolygon(polygon);
        }

        // 车道
        painter->setPen(QPen(Qt::blue, 2));
        for (const auto& it : m_contour.lanes)
        {
            const TIDLane& lane = it.second;
            // 车道区域
            QPolygon polygon(lane.pts.size());
            for (int i = 0; i < lane.pts.size(); i++)
            {
                polygon[i] = toPixelPoint(lane.pts[i], size);
            }
            painter->drawPolygon(polygon);
            
            // 车道方向线
            if (lane.direction.size() == 2)
            {
                QPoint start = toPixelPoint(lane.direction[0], size);
                QPoint end = toPixelPoint(lane.direction[1], size);
                painter->drawLine(start, end);
            }
        }

        // 车道虚拟线圈
        painter->setPen(QPen(Qt::red, 1.5, Qt::DashDotLine));
        for (const auto& it : m_contour.lanes)
        {
            const auto& loop = it.second.virtualLoop;
            QPolygon polygon(loop.size());
            for (int i = 0; i < loop.size(); i++)
            {
                polygon[i] = toPixelPoint(loop[i], size);
            }
            painter->drawPolygon(polygon);
        }

        // 绘制中的图形
        if (vecPointCache.size() > 0)
        {
            if (mode == "lane")
            {
                painter->setPen(QPen(Qt::blue, 2));
            }
            else if (mode == "region")
            {
                painter->setPen(QPen(Qt::yellow, 2));
            }
            if (vecPointCache.size() > 1)
            {
                QPolygon polygon(vecPointCache.size());
                for (int i = 0; i < vecPointCache.size(); i++)
                {
                    polygon[i] = toPixelPoint(vecPointCache[i], size);
                }
                painter->drawPolyline(polygon);
            }
            if (ptCache.x() != 0 || ptCache.y() != 0)
            {
                QPoint pt1 = toPixelPoint(vecPointCache.back(), size);
                QPoint pt2 = toPixelPoint(ptCache, size);
                painter->drawLine(pt1, pt2);
            }
        }
    }
}
