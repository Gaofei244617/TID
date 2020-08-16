#pragma once

#include <unordered_map>
#include <QString>
#include <QPoint>
#include <QVector>
#include <QPoint>
#include <QSize>
#include <QPolygon>
#include <QLine>
#include "rapidjson/document.h"

struct TIDLane
{
	QString type;
	QLine direction;
	QPolygon lane;
	QPolygon virtualLoop;
};

struct TIDRegion
{
	QPolygon region;
};

struct TIDContour
{
	std::unordered_map<int, TIDLane> lanes;
	std::unordered_map<int, TIDRegion> regions;
	QString toJsonString()const;
};

QPoint toRelativePoint(const QPoint& pt, const QSize& size);
QPoint toPixelPoint(const QPoint& pt, const QSize& size);
QPolygon toPixelPolygon(const QPolygon& polygon, const QSize& size);
QLine toPixelLine(const QLine& line, const QSize& size);

// 获取方向线所在车道ID
int getLaneID(const QLine& line, const TIDContour& contour);
// 获取虚拟线圈所在车道ID
int getLaneID(const QPolygon& polygon, const TIDContour& contour);

QVector<QPointF> getArrow(const QLineF& line);

QString JsonToString(const rapidjson::Document& doc);
