#include "common.h"
#include <algorithm>
#include "rapidjson/prettywriter.h"  
#include "rapidjson/writer.h"  
#include "rapidjson/stringbuffer.h"

QString JsonToString(const rapidjson::Document& doc)
{
    rapidjson::StringBuffer jsonBuffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(jsonBuffer);
    doc.Accept(writer);
    return jsonBuffer.GetString();
}

QString JsonToPrettyString(const rapidjson::Document& doc)
{
    rapidjson::StringBuffer jsonBuffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(jsonBuffer);
    writer.SetFormatOptions(rapidjson::kFormatSingleLineArray);
    doc.Accept(writer);
    return jsonBuffer.GetString();
}

QString JsonToPrettyString2(const rapidjson::Document& doc)
{
    rapidjson::StringBuffer jsonBuffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(jsonBuffer);
    doc.Accept(writer);
    return jsonBuffer.GetString();
}

QString TIDContour::toJsonString()const
{
    using namespace rapidjson;
    Document obj;
    obj.SetObject();
    auto& allo = obj.GetAllocator();

    Value laneArry(kArrayType);
    for (const auto& tidLane : lanes)
    {
        Value laneObj(kObjectType);
        laneObj.AddMember("Id", tidLane.first, allo);
        laneObj.AddMember("Type", "BusLane", allo);

        Value lane(kArrayType);
        for (const auto& pt : tidLane.second.lane)
        {
            lane.PushBack(pt.x(), allo);
            lane.PushBack(pt.y(), allo);
        }
        laneObj.AddMember("Coordinates", lane, allo);

        const auto& line = tidLane.second.direction;
        Value start(kArrayType);
        start.PushBack(line.x1(), allo);
        start.PushBack(line.y1(), allo);
        Value end(kArrayType);
        end.PushBack(line.x2(), allo);
        end.PushBack(line.y2(), allo);
        Value direct(kObjectType);
        direct.AddMember("Start", start, allo);
        direct.AddMember("End", end, allo);
        laneObj.AddMember("Direction", direct, allo);

        Value loop(kArrayType);
        for (const auto& pt : tidLane.second.virtualLoop)
        {
            loop.PushBack(pt.x(), allo);
            loop.PushBack(pt.y(), allo);
        }
        laneObj.AddMember("VirtualLoop", loop, allo);
        laneArry.PushBack(laneObj, allo);
    }
    obj.AddMember("Lane", laneArry, allo);

    Value regionArry(kArrayType);
    for (const auto& tidRegion : regions)
    {
        Value regionObj(kObjectType);
        regionObj.AddMember("Id", tidRegion.first, allo);
        Value region(kArrayType);
        for (const auto& pt : tidRegion.second.region)
        {
            region.PushBack(pt.x(), allo);
            region.PushBack(pt.y(), allo);
        }
        regionObj.AddMember("Coordinates", region, allo);

        /************************************************************/
        Value incident(kArrayType);
        incident.PushBack("Amble", allo);
        incident.PushBack("FireSmoke", allo);
        incident.PushBack("Retrograde", allo);
        incident.PushBack("AbandonedObject", allo);
        incident.PushBack("Accident", allo);
        incident.PushBack("IllegalParking", allo);
        incident.PushBack("OccupancyEL", allo);
        incident.PushBack("NonvehicleEntry", allo);
        incident.PushBack("Congestion", allo);
        incident.PushBack("BearOff", allo);
        incident.PushBack("Fog", allo);
        incident.PushBack("Snow", allo);
        regionObj.AddMember("Incident", incident, allo);

        Value incidentParam(kObjectType);
        incidentParam.AddMember("AbandonedObjectSense", 60, allo);
        incidentParam.AddMember("CongestionLength", 90, allo);
        incidentParam.AddMember("CongestionInterval", 30, allo);
        incidentParam.AddMember("IllegalParkTime", 20, allo);
        regionObj.AddMember("IncidentParm", incidentParam, allo);
        /************************************************************/
        regionArry.PushBack(regionObj, allo);
    }
    obj.AddMember("AnalysisRegion", regionArry, allo);
    return JsonToPrettyString(obj);
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
