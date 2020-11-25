#pragma once

#include <QGraphicsView>
#include <QPoint>
#include <QMouseEvent>
#include <QString>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QDropEvent>
#include <QUrl>
#include <QGraphicsScene>
#include <QResizeEvent>
#include <opencv2/opencv.hpp>
#include "common.h"
#include "MyQGraphicsItem.h"
#include <QGraphicsPixmapItem>

class MyQGraphicsView : public QGraphicsView
{
	Q_OBJECT 
public:
	explicit MyQGraphicsView(QWidget* parent = nullptr);

	void onSliderChangeed(int value);
	void setImage(const cv::Mat& frame);
	void setRegionMode(int mode);
	void clearContour();
	void actionOnOpenFile(QString filePath);
	void setContour(const QString& str);
	cv::Mat getCurrentFrame()const;

	void mouseMoveEvent(QMouseEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void dragEnterEvent(QDragEnterEvent* event); //拖动进入事件
	void dropEvent(QDropEvent* event);
	void resizeEvent(QResizeEvent* event);        // 缩放事件

signals:
	void mouseMoveSignal(QPoint pt);
	void openFileSignal(QString str);
	void updateJsonSignal(QString str);

protected:
	QGraphicsScene* scene;
	QGraphicsPixmapItem* pixItem;
	QGraphicsItem* item;
	QString filePath;
	cv::VideoCapture cap;
	double frameNum;       // 视频总帧数
	cv::Mat frame;         // 当前视频帧
	QString drawMode;
	TIDContour m_contour;  // 相对坐标
	QPoint* cpt;           // 鼠标选中的点
	bool dragPointFlag;    // 上一操作是否为拖拽顶点操作
	QVector<QPoint> m_mesureData;  // 测量点

private:
	QVector<QPoint> vecPointCache;
	QPoint ptCache;
};
