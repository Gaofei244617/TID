#pragma once

#include <QGraphicsView>
#include <QPoint>
#include <QMouseEvent>
#include <QString>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QGraphicsScene>
#include <QGraphicsSceneDragDropEvent>

class MyQGraphicsScene : public QGraphicsScene
{
	Q_OBJECT

public:
	explicit MyQGraphicsScene(QWidget* parent = nullptr);
	void dragMoveEvent(QGraphicsSceneDragDropEvent* event);
};
