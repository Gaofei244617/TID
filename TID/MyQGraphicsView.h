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
	void dragEnterEvent(QDragEnterEvent* event); //�϶������¼�
	void dropEvent(QDropEvent* event);
	void resizeEvent(QResizeEvent* event);        // �����¼�

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
	double frameNum;       // ��Ƶ��֡��
	cv::Mat frame;         // ��ǰ��Ƶ֡
	QString drawMode;
	TIDContour m_contour;  // �������
	QPoint* cpt;           // ���ѡ�еĵ�
	bool dragPointFlag;    // ��һ�����Ƿ�Ϊ��ק�������
	QVector<QPoint> m_mesureData;  // ������

private:
	QVector<QPoint> vecPointCache;
	QPoint ptCache;
};
