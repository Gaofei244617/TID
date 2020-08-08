#include "MyQGraphicsScene.h"

MyQGraphicsScene::MyQGraphicsScene(QWidget* parent)
    :QGraphicsScene(parent)
{}

void MyQGraphicsScene::dragMoveEvent(QGraphicsSceneDragDropEvent* event)
{
    event->accept();
}
