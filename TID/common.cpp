#include "common.h"
#include <algorithm>
#include "rapidjson/prettywriter.h"  
#include "rapidjson/writer.h"  
#include "rapidjson/stringbuffer.h"
#include <QMessageBox>
#include <QDomDocument>

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

// 解析目标框
QVector<BndBox> getBndBox(QFile* file)
{
    QVector<BndBox> boxes;
    QDomDocument doc;
    if (!doc.setContent(file, false))
    {
        QMessageBox::warning(nullptr, nullptr, "Can not parse this xml file\n");
        return boxes;
    }

    QDomElement root = doc.documentElement();
    if (root.tagName() != "annotation")
    {
        return boxes;
    }

    QDomNode child = root.firstChild();
    while (!child.isNull())
    {
        if (child.toElement().tagName() == "object")
        {
            BndBox box;
            QDomNode sub_child = child.toElement().firstChild();
            while (!sub_child.isNull())
            {
                if (sub_child.toElement().tagName() == "name")
                {
                    box.name = sub_child.toElement().text();
                }
                else if (sub_child.toElement().tagName() == "bndbox")
                {
                    QDomNode sub2_child = sub_child.toElement().firstChild();
                    while (!sub2_child.isNull())
                    {
                        QString tagName = sub2_child.toElement().tagName();
                        if (tagName == "xmax")
                        {
                            box.xmax = sub2_child.toElement().text().toInt();
                        }
                        else if (tagName == "xmin")
                        {
                            box.xmin = sub2_child.toElement().text().toInt();
                        }
                        else if (tagName == "ymax")
                        {
                            box.ymax = sub2_child.toElement().text().toInt();
                        }
                        else if (tagName == "ymin")
                        {
                            box.ymin = sub2_child.toElement().text().toInt();
                        }
                        sub2_child = sub2_child.nextSibling();
                    }
                }
                sub_child = sub_child.nextSibling();
            }
            boxes.push_back(box);
        }
        child = child.nextSibling();
    }

    return boxes;
}

QVector<BndBox> getBndBox(QFile* file, const QSize& size)
{
    QVector<BndBox> boxes;
    int width = size.width();
    int height = size.height();

    QDomDocument doc;
    if (!doc.setContent(file, false))
    {
        QMessageBox::warning(nullptr, nullptr, "Can not parse this xml file\n");
        return boxes;
    }

    QDomElement root = doc.documentElement();
    if (root.tagName() != "annotation")
    {
        return boxes;
    }

    QDomNode child = root.firstChild();
    while (!child.isNull())
    {
        if (child.toElement().tagName() == "object")
        {
            BndBox box;
            QDomNode sub_child = child.toElement().firstChild();
            while (!sub_child.isNull())
            {
                if (sub_child.toElement().tagName() == "name")
                {
                    box.name = sub_child.toElement().text();
                }
                else if (sub_child.toElement().tagName() == "bndbox")
                {
                    QDomNode sub2_child = sub_child.toElement().firstChild();
                    while (!sub2_child.isNull())
                    {
                        int num = 0;
                        QString tagName = sub2_child.toElement().tagName();
                        if (tagName == "xmax")
                        {
                            num = sub2_child.toElement().text().toInt();
                            box.xmax = static_cast<int>(num * 10000.0 / width);
                        }
                        else if (tagName == "xmin")
                        {
                            num = sub2_child.toElement().text().toInt();
                            box.xmin = static_cast<int>(num * 10000.0 / width);
                        }
                        else if (tagName == "ymax")
                        {
                            num = sub2_child.toElement().text().toInt();
                            box.ymax = static_cast<int>(num * 10000.0 / height);
                        }
                        else if (tagName == "ymin")
                        {
                            num = sub2_child.toElement().text().toInt();
                            box.ymin = static_cast<int>(num * 10000.0 / height);
                        }
                        sub2_child = sub2_child.nextSibling();
                    }
                }
                sub_child = sub_child.nextSibling();
            }
            boxes.push_back(box);
        }
        child = child.nextSibling();
    }

    return boxes;
}

// 计算两直线夹角(0~90°)
double calAngle(const QLine& line1, const QLine& line2)
{
    const double PI = 3.141592654;
    const double Error = 0.00001;
    double x1 = line1.dx();
    double y1 = line1.dy();
    double x2 = line2.dx();
    double y2 = line2.dy();

    // 夹角余弦值
    auto temp = (x1 * x2 + y1 * y2) / (std::sqrt(x1 * x1 + y1 * y1) * std::sqrt(x2 * x2 + y2 * y2));
    // 处理浮点数误差
    if (std::abs(temp) < Error)
    {
        return 90.0;
    }
    double theta = std::acos(temp) / PI * 180.0;
    theta = theta > 90.0 ? 180.0 - theta : theta;
    return theta;
}

// 两条线段是否首尾相连
bool isBeside(const QLine& line1, const QLine& line2)
{
    bool ret = false;
    if (line1.p1() == line2.p1() ||
        line1.p1() == line2.p2() ||
        line1.p2() == line2.p1() ||
        line1.p2() == line2.p2())
    {
        ret = true;
    }
    return ret;
}

// 均分线段(返回值包含端点)
std::vector<QPoint> divLine(const QLine& line, const int num)
{
    QPoint start = line.p1();
    QPoint end = line.p2();
    int x1 = start.x();
    int y1 = start.y();
    int dx = line.dx();
    int dy = line.dy();

    std::vector<QPoint> pts;
    pts.reserve(num + 1);
    pts.push_back(start);
    for (int i = 1; i < num; i++)
    {
        pts.emplace_back(x1 + dx * i / num, y1 + dy * i / num);
    }
    pts.push_back(end);

    return pts;
}

// 计算虚拟线圈
QPolygon calVirtualLoop(const QPolygon& lane, const Direct& direct)
{
    const QLine laneDirect(direct.start, direct.end);  // 车道方向
    std::vector<std::pair<QLine, double>> edges;
    for (int i = 0, j = lane.size() - 1; i < lane.size(); i++)
    {
        QLine edge(lane[j], lane[i]);
        edges.push_back(std::make_pair(edge, calAngle(edge, laneDirect)));
        j = i;
    }
    using T = std::pair<QLine, double>;
    std::sort(edges.begin(), edges.end(), [](const T& a, const T& b) {return a.second < b.second; }); // 升序
    
    QLine line1 = edges[0].first;
    QLine line2 = edges[1].first;
    // 避免与车道方向夹角最小的两条边是相邻的边
    if (isBeside(line1, line2))
    {
        for (int i = 2; i < edges.size(); i++)
        {
            if (!isBeside(line1, edges[i].first))
            {
                line2 = edges[i].first;
                break;
            }
        }
    }

    // 保证两条边端点在y方向上顺序相同
    if (line1.y1() < line1.y2() != line2.y1() < line2.y2())
    {
        QPoint tmp = line2.p1();
        line2 = QLine(line2.p2(), line2.p1());
    }
    int num = 50;
    int w = num / 7;
    if ((num - w) % 2 != 0)
    {
        w++;
    }
    auto pts1 = divLine(line1, num);
    auto pts2 = divLine(line2, num);
    QPoint pt1 = pts1.at((num - w) / 2);
    QPoint pt2 = pts1.at((num - w) / 2 + w);

    double ang = 0;
    QPoint pt4;
    for (const auto& pt : pts2)
    {
        double tmp = calAngle(QLine(pt1, pt), laneDirect);
        if (tmp > ang)
        {
            ang = tmp;
            pt4 = pt;
        }
    }

    ang = 0;
    QPoint pt3;
    for (const auto& pt : pts2)
    {
        double tmp = calAngle(QLine(pt2, pt), laneDirect);
        if (tmp > ang)
        {
            ang = tmp;
            pt3 = pt;
        }
    }

    QPolygon polygon({ pt1,pt2,pt3,pt4 });

    return polygon;
}