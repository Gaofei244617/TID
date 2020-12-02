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
#include <tuple>
#include <QFile>
#include <opencv2/opencv.hpp>

enum class ContourType
{
	NONE,
	LANE,
	VIRTUAL_LOOP,
	ARROW,
	REGION
};

struct Direct
{
	QPoint start;
	QPoint end;
	Direct() :start(QPoint(0, 0)), end(QPoint(0, 0)) {}
	Direct(const QPoint& pt1, const QPoint& pt2) :start(pt1), end(pt2) {}
	Direct(const Direct& direct) :start(direct.start), end(direct.end) {}
	bool isNull()const { return start == QPoint(0, 0) && end == QPoint(0, 0); }
};

struct TIDLane
{
	QString type;
	Direct direction;
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

struct BndBox
{
	QString name;
	int xmin;
	int xmax;
	int ymin;
	int ymax;
	BndBox() :xmin(0), xmax(0), ymin(0), ymax(0) {}
};

QPoint toRelativePoint(const QPoint& pt, const QSize& size);
QPoint toPixelPoint(const QPoint& pt, const QSize& size);
QPolygon toPixelPolygon(const QPolygon& polygon, const QSize& size);
Direct toPixelLine(const Direct& line, const QSize& size);

// ��ȡ���������ڳ���ID
int getLaneID(const Direct& line, const TIDContour& contour);
// ��ȡ������Ȧ���ڳ���ID
int getLaneID(const QPolygon& polygon, const TIDContour& contour);

// ��ȡ��ͷ
QVector<QPointF> getArrow(const QLineF& line, const int size);

TIDContour getTIDContour(const QString& json);

QString JsonToString(const rapidjson::Document& doc);
QString JsonToPrettyString(const rapidjson::Document& doc);
QString JsonToPrettyString2(const rapidjson::Document& doc);

// ���������
std::tuple<QPoint*, double> findPoint(const TIDContour& contour, const QPoint& pt);

double square(const double num);

// ������������
double distance(const QPoint& pt1, const QPoint& pt2);

// ����Ŀ���
QVector<BndBox> getBndBox(QFile* file);

// ����ͼƬ�ϻ���Ŀ���
void drawBox(const QVector<BndBox>& boxes, cv::Mat& img);
