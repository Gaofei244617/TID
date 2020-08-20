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

// 绘图
void MyQGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (m_contour.lanes.size() > 0 || m_contour.regions.size() > 0 || vecPointCache.size() > 0)
    {
        painter->setRenderHints(QPainter::Antialiasing, true); // 抗锯齿
        painter->setFont(QFont("times", 18));
        auto size = this->scene()->views().at(0)->size();
        // 分析区域
        painter->setPen(QPen(Qt::yellow, 2));
        for (const auto& it : m_contour.regions)
        {
            const TIDRegion& tidRegion = it.second;
            painter->drawPolygon(toPixelPolygon(tidRegion.region, size));
            painter->drawText(toPixelPoint(tidRegion.region.at(0), size), QString("#%1").arg(it.first));
        }

        // 车道
        painter->setPen(QPen(Qt::blue, 2));
        for (const auto& it : m_contour.lanes)
        {
            const TIDLane& tidLane = it.second;
            // 车道区域
            painter->drawPolygon(toPixelPolygon(tidLane.lane, size));
            painter->drawText(toPixelPoint(tidLane.lane.at(0), size), QString("#%1").arg(it.first));
            
            // 车道方向线
            if (!tidLane.direction.isNull())
            {
                auto line = QLine(toPixelLine(tidLane.direction, size));
                painter->drawLine(line);
                // 绘制箭头
                auto pts = getArrow(line, 18);
                QPolygon polygon;
                polygon << line.p2() << pts[0].toPoint() << pts[1].toPoint();
                QPainterPath path;
                path.addPolygon(polygon);
                painter->fillPath(path, QBrush(Qt::blue));
            }
        }

        // 车道虚拟线圈
        painter->setPen(QPen(Qt::red, 1.5, Qt::DashDotLine));
        for (const auto& it : m_contour.lanes)
        {
            const auto& loop = it.second.virtualLoop;
            painter->drawPolygon(toPixelPolygon(loop, size));
        }

        // 绘制中的图形
        if (vecPointCache.size() > 0)
        {
            if (mode == "lane" || mode == "direction")
            {
                painter->setPen(QPen(Qt::blue, 2));
            }
            else if (mode == "region")
            {
                painter->setPen(QPen(Qt::yellow, 2));
            }
            else if (mode == "loop")
            {
                painter->setPen(QPen(Qt::red, 1.5, Qt::DashDotLine));
            }
            if (vecPointCache.size() > 1)
            {
                painter->drawPolyline(toPixelPolygon(QPolygon(vecPointCache), size));
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
