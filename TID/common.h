#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <QPoint>
#include <QVector>
#include <QPoint>
#include <QSize>

struct TIDLane
{
	std::string type;
	QVector<QPoint> direction;
	QVector<QPoint> pts;
	QVector<QPoint> virtualLoop;
};

struct TIDRegion
{
	QVector<QPoint> pts;
};

struct TIDContour
{
	std::unordered_map<int, TIDLane> lanes;
	std::unordered_map<int, TIDRegion> regions;
};

QPoint toRelativePoint(const QPoint& pt, const QSize& size);
QPoint toPixelPoint(const QPoint& pt, const QSize& size);

