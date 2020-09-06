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

    auto size = this->scene()->views().at(0)->size();
    if (m_contour.lanes.size() > 0 || m_contour.regions.size() > 0 || vecPointCache.size() > 0)
    {
        QColor orange(0xff, 0x7f, 0);
        painter->setRenderHints(QPainter::Antialiasing, true); // 抗锯齿
        painter->setFont(QFont("times", 18));
       
        // 分析区域
        for (const auto& it : m_contour.regions)
        {
            painter->setPen(QPen(Qt::yellow, 2));
            const TIDRegion& tidRegion = it.second;
            auto polygon = toPixelPolygon(tidRegion.region, size);
            painter->drawPolygon(polygon);
            painter->drawText(toPixelPoint(tidRegion.region.at(0), size), QString("#%1").arg(it.first));
            painter->setPen(QPen(Qt::green, 5));
            painter->drawPoints(polygon);
        }

        // 车道
        for (const auto& it : m_contour.lanes)
        {
            const TIDLane& tidLane = it.second;
            if (tidLane.type == "BusLane")
            {
                painter->setPen(QPen(Qt::blue, 2));
            }
            else if (tidLane.type == "EmergencyLane")
            {
                painter->setPen(QPen(orange, 2));
            }

            // 车道区域
            auto polygon = toPixelPolygon(tidLane.lane, size);
            painter->drawPolygon(polygon);
            painter->drawText(toPixelPoint(tidLane.lane.at(0), size), QString("#%1").arg(it.first));
            painter->setPen(QPen(Qt::green, 5));
            painter->drawPoints(polygon);
            
            // 车道方向线
            if (!tidLane.direction.isNull())
            {
                auto line = QLine(toPixelLine(tidLane.direction, size));
                QBrush brush = tidLane.type == "BusLane" ? QBrush(Qt::blue) : QBrush(orange);
                painter->setPen(QPen(brush, 2, Qt::SolidLine, Qt::FlatCap));

                // 绘制箭头
                painter->drawLine(line);
                auto pts = getArrow(line, 18);
                QPolygon polygon;
                polygon << line.p2() << pts[0].toPoint() << pts[1].toPoint();
                QPainterPath path;
                path.addPolygon(polygon);
                painter->fillPath(path, brush);
            }
        }

        // 车道虚拟线圈
        for (const auto& it : m_contour.lanes)
        {
            painter->setPen(QPen(Qt::red, 1.5, Qt::DashDotLine));
            const auto& loop = it.second.virtualLoop;
            auto polygon = toPixelPolygon(loop, size);
            painter->drawPolygon(polygon);
            painter->setPen(QPen(orange, 4));
            painter->drawPoints(polygon);
        }

        // 绘制中的图形
        if (vecPointCache.size() > 0)
        {
            if (mode == "BusLane")
            {
                painter->setPen(QPen(Qt::blue, 2));
            }
            else if (mode == "EmergencyLane")
            {
                painter->setPen(QPen(orange, 2));
            }
            else if (mode == "direction")
            {
                for (const auto& it : m_contour.lanes)
                {
                    if (it.second.lane.containsPoint(vecPointCache.back(), Qt::OddEvenFill))
                    {
                        QColor color = it.second.type == "BusLane" ? QColor(Qt::blue) : orange;
                        painter->setPen(QPen(color, 2));
                        break;
                    }
                }
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

    painter->setPen(QPen(Qt::gray, 0.5, Qt::DotLine));
    auto pt = toPixelPoint(ptCache, size);
    painter->drawLine(QPoint(0, pt.y()), QPoint(size.width(), pt.y()));
    painter->drawLine(QPoint(pt.x(), 0), QPoint(pt.x(), size.height()));
}
