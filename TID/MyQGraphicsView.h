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
	void releaseOnRightBtn();  // 鼠标右键释放响应函数

	void mouseMoveEvent(QMouseEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void dragEnterEvent(QDragEnterEvent* event);  //拖动进入事件
	void dropEvent(QDropEvent* event);
	void resizeEvent(QResizeEvent* event);        // 缩放事件

signals:
	void mouseMoveSignal(QPoint pt, QPoint pt2);
	void openFileSignal(QString str);
	void updateJsonSignal(QString str);

public:
	QGraphicsScene* m_scene;

protected:
	QGraphicsPixmapItem* m_pixItem;
	QGraphicsItem* m_item;
	QString m_filePath;
	cv::VideoCapture m_cap;
	double m_frameNum;       // 视频总帧数
	cv::Mat m_frame;         // 当前视频帧
	QString m_drawMode;
	TIDContour m_contour;    // 相对坐标
	QPoint* m_cpt;           // 鼠标选中的点
	bool m_dragPointFlag;    // 上一操作是否为拖拽顶点操作
	QVector<QPoint> m_mesureData;  // 测量点

private:
	QVector<QPoint> m_vecPointCache;
	QPoint m_ptCache;
};
