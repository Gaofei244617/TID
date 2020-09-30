#include "common.h"
#include <algorithm>
#include "rapidjson/prettywriter.h"  
#include "rapidjson/writer.h"  
#include "rapidjson/stringbuffer.h"
#include <QMessageBox>

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
    Document doc;
    doc.SetObject();
    auto& allo = doc.GetAllocator();

    doc.AddMember("channelId", "0", allo);

    Value cfg(kObjectType);
    /****************************************/
    cfg.AddMember("ImageWidth", 1920, allo);
    cfg.AddMember("ImageHeight", 1080, allo);
    cfg.AddMember("SceneMode", "MainRoad", allo);
    /****************************************/

    Value laneArry(kArrayType);
    for (const auto& tidLane : lanes)
    {
        Value laneObj(kObjectType);
        laneObj.AddMember("Id", tidLane.first, allo);

        Value laneType(kObjectType);
        laneType.SetString(tidLane.second.type.toStdString().c_str(), allo);
        laneObj.AddMember("Type", laneType, allo);

        Value lane(kArrayType);
        for (const auto& pt : tidLane.second.lane)
        {
            lane.PushBack(pt.x(), allo);
            lane.PushBack(pt.y(), allo);
        }
        laneObj.AddMember("Coordinates", lane, allo);

        const auto& line = tidLane.second.direction;
        Value start(kArrayType);
        start.PushBack(line.start.x(), allo);
        start.PushBack(line.start.y(), allo);
        Value end(kArrayType);
        end.PushBack(line.end.x(), allo);
        end.PushBack(line.end.y(), allo);
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
    cfg.AddMember("Lane", laneArry, allo);

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
        incident.PushBack("NonVehicleEntry", allo);
        incident.PushBack("PedestrianEntry", allo);
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
        regionObj.AddMember("IncidentParam", incidentParam, allo);
        /************************************************************/
        regionArry.PushBack(regionObj, allo);
    }
    cfg.AddMember("AnalysisRegion", regionArry, allo);

    /************************************************************/
    cfg.AddMember("ReportingInterval", 60, allo);
    /************************************************************/
    doc.AddMember("config", cfg, allo);

    return JsonToPrettyString(doc);
}

TIDContour getTIDContour(const QString& json)
{
    rapidjson::Document doc;
    doc.Parse(json.toStdString().c_str());
    if (doc.HasParseError())
    {
        QMessageBox::critical(nullptr, nullptr, "Fail to Parse TID Json!");
        return TIDContour();
    }

    TIDContour contour;
    for (auto it = doc["config"]["Lane"].Begin(); it != doc["config"]["Lane"].End(); it++)
    {
        auto lane = it->GetObject();

        int id = lane["Id"].GetInt();
        QString type = lane["Type"].GetString();

        QPolygon polygon;
        for (int i = 0; i < lane["Coordinates"].Size(); i += 2)
        {
            polygon.append(QPoint(lane["Coordinates"][i].GetInt(), lane["Coordinates"][i + 1].GetInt()));
        }

        QPoint start(lane["Direction"]["Start"][0].GetInt(), lane["Direction"]["Start"][1].GetInt());
        QPoint end(lane["Direction"]["End"][0].GetInt(), lane["Direction"]["End"][1].GetInt());
        Direct direction(start, end);

        QPolygon virtualLoop;
        for (int i = 0; i < lane["VirtualLoop"].Size(); i += 2)
        {
            virtualLoop.append(QPoint(lane["VirtualLoop"][i].GetInt(), lane["VirtualLoop"][i + 1].GetInt()));
        }
        contour.lanes[id] = TIDLane{ type, direction, polygon, virtualLoop };
    }

    for (auto it = doc["config"]["AnalysisRegion"].Begin(); it != doc["config"]["AnalysisRegion"].End(); it++)
    {
        auto region = it->GetObject();
        int id = region["Id"].GetInt();
        QPolygon polygon;
        for (int i = 0; i < region["Coordinates"].Size(); i += 2)
        {
            polygon.append(QPoint(region["Coordinates"][i].GetInt(), region["Coordinates"][i + 1].GetInt()));
        }
        contour.regions[id] = TIDRegion{ polygon };
    }

    return contour;
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

Direct toPixelLine(const Direct& line, const QSize& size)
{
    if (line.isNull())
    {
        return Direct();
    }
    return Direct(toPixelPoint(line.start, size), toPixelPoint(line.end, size));
}

int getLaneID(const Direct& line, const TIDContour& contour)
{
    int laneID = -1;
    for (const auto& it : contour.lanes)
    {
        int id = it.first;
        const auto& lane = it.second.lane;
        if (lane.containsPoint(line.start, Qt::OddEvenFill) &&
            lane.containsPoint(line.end, Qt::OddEvenFill))
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
QVector<QPointF> getArrow(const QLineF& line, const int arrowSize)
{
    const double PI = 3.14159265358979323846264338327950288419717;
    double angle = acos(line.dx() / line.length());

    if (line.dy() >= 0)
    {
        angle = 2 * PI - angle;
    }
    QPointF pt1 = line.p2() + QPointF(sin(angle - PI / 3) * arrowSize, cos(angle - PI / 3) * arrowSize);
    QPointF pt2 = line.p2() + QPointF(sin(angle - 2 * PI / 3) * arrowSize, cos(angle - 2 * PI / 3) * arrowSize);

    return QVector<QPointF>{pt1, pt2};
}

double square(const double num)
{
    return num * num;
}

double distance(const QPoint& pt1, const QPoint& pt2)
{
    return sqrt(double(square(pt2.x() - pt1.x()) + square(pt2.y() - pt1.y())));
}

// 查找最近点
std::tuple<QPoint*, double> findPoint(const TIDContour& contour, const QPoint& pt)
{
    QPoint* point = nullptr;
    double dist = 0xffff;

    // 车道
    for (const auto& it : contour.lanes)
    {
        // 车道
        const auto& lanePts = it.second.lane;
        for (const auto& p : lanePts)
        {
            double d = distance(p, pt);
            if (d < dist)
            {
                point = const_cast<QPoint*>(&p);
                dist = d;
            }
        }

        // 虚拟线圈
        const auto& loopPts = it.second.virtualLoop;
        for (const auto& p : loopPts)
        {
            double d = distance(p, pt);
            if (d < dist)
            {
                point = const_cast<QPoint*>(&p);
                dist = d;
            }
        }

        // 方向线
        const auto& start = it.second.direction.start;
        const auto& end = it.second.direction.end;
        double d = distance(start, pt);
        if (d < dist)
        {
            point = const_cast<QPoint*>(&start);
            dist = d;
        }
        d = distance(end, pt);
        if (d < dist)
        {
            point = const_cast<QPoint*>(&end);
            dist = d;
        }
    }

    // 分析区域
    for (const auto& it : contour.regions)
    {
        const auto& pts = it.second.region;
        for (const auto& p : pts)
        {
            double d = distance(p, pt);
            if (d < dist)
            {
                point = const_cast<QPoint*>(&p);
                dist = d;
            }
        }
    }

    return std::make_tuple(point, dist);
}
