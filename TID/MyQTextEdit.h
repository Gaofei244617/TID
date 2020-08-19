#pragma once

#include <QTextEdit>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>

class MyQTextEdit :public QTextEdit
{
	Q_OBJECT

public:
	explicit MyQTextEdit(QWidget* parent = nullptr);
	void dragEnterEvent(QDragEnterEvent* event); //�϶������¼�
	void dragMoveEvent(QDragMoveEvent* event); 
	void dropEvent(QDropEvent* event);

signals:
	void inputFileSignal(QString str);
};