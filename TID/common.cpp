#include "common.h"
#include <algorithm>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>

QString TIDContour::toJsonString()const
{
    QJsonObject obj;
    QJsonArray laneArry;
    for (const auto& tidLane : lanes)
    {
        QJsonObject laneObj;

        laneObj.insert("ID", tidLane.first);
        laneObj.insert("Type", "BusLane");

        QJsonArray lane;
        for (const auto& pt : tidLane.second.lane)
        {
            lane.append(pt.x());
            lane.append(pt.y());
        }
        laneObj.insert("Coordinate", lane);

        QJsonObject direct;
        const auto& line = tidLane.second.direction;
        QJsonArray start = { line.x1(), line.y1()};
        QJsonArray end = { line.x2(), line.y2() };
        direct.insert("Start", start);
        direct.insert("End", end);
        laneObj.insert("Direction", direct);
        
        QJsonArray loop;
        for (const auto& pt : tidLane.second.virtualLoop)
        {
            loop.append(pt.x());
            loop.append(pt.y());
        }
        laneObj.insert("VirtualLoop", loop);
        laneArry.append(laneObj);
    }
    obj.insert("Lane", laneArry);

    QJsonArray regionArry;
    for (const auto& tidRegion : regions)
    {
        QJsonObject regionObj;
        regionObj.insert("ID", tidRegion.first);
        QJsonArray region;
        for (const auto& pt : tidRegion.second.region)
        {
            region.append(pt.x());
            region.append(pt.y());
        }
        regionObj.insert("Coordinate", region);
        regionObj.insert("ID", tidRegion.first);
        regionObj.insert("ID", tidRegion.first);

        regionArry.append(regionObj);
    }
    obj.insert("AnalysisRegion", regionArry);

    QJsonDocument doc(obj);
    return doc.toJson(QJsonDocument::Indented);
}

QPoint toRelativePoint(const QPoint& pt, const QSize& size)
{
    int x = static_cast<int>(pt.x() * 10000.0 / size.width());
    int y = static_cast<int>(pt.y() * 10000.0 / size.height());
    return QPoint(x, y);
}

QPoint toPixelPoint(const QPoint& pt, const QSize& size)
{
    int x = static_cast<int>(pt.x() / 10000.0 * size.width());
    int y = static_cast<int>(pt.y() / 10000.0 * size.height());
    return QPoint(x, y);
}

QPolygon toPixelPolygon(const QPolygon& polygon, const QSize& size)
{
    QPolygon pixelPolygon(polygon.size());
    std::transform(polygon.cbegin(), polygon.cend(), pixelPolygon.begin(), 
        [&size](const QPoint& pt) {return toPixelPoint(pt, size); });
    return pixelPolygon;
}

QLine toPixelLine(const QLine& line, const QSize& size)
{
    if (line.isNull())
    {
        return QLine();
    }
    return QLine(toPixelPoint(line.p1(), size), toPixelPoint(line.p2(), size));
}

int getLaneID(const QLine& line, const TIDContour& contour)
{
    int laneID = -1;
    for (const auto& it : contour.lanes)
    {
        int id = it.first;
        const auto& lane = it.second.lane;
        if (lane.containsPoint(line.p1(), Qt::OddEvenFill) &&
            lane.containsPoint(line.p2(), Qt::OddEvenFill))
        {
            laneID = id;
            break;
        }
    }
    return laneID;
}

int getLaneID(const QPolygon& polygon, const TIDContour& contour)
{
    int laneID = -1;
    for (const auto& it : contour.lanes)
    {
        int id = it.first;
        const auto& lane = it.second.lane;
        // 大于等于3个定点在车道区域内则认为虚拟线圈在该车道内
        int cnt = 0;
        for (const auto& pt : polygon)
        {
            if (lane.containsPoint(pt, Qt::OddEvenFill))
            {
                cnt++;
            }
            if (cnt >= 3)
            {
                laneID = id;
                break;
            }
        }
    }
    return laneID;
}

// 计算直线末端箭头
QVector<QPointF> getArrow(const QLineF& line)
{
    const double PI = 3.14159265358979323846264338327950288419717;
    const int arrowSize = 15;
    double angle = acos(line.dx() / line.length());

    if (line.dy() >= 0)
    {
        angle = 2 * PI - angle;
    }
    QPointF pt1 = line.p2() + QPointF(sin(angle - PI / 3) * arrowSize, cos(angle - PI / 3) * arrowSize);
    QPointF pt2 = line.p2() + QPointF(sin(angle - 2 * PI / 3) * arrowSize, cos(angle - 2 * PI / 3) * arrowSize);

    return QVector<QPointF>{pt1, pt2};
}

