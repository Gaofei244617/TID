#include "MyQTextEdit.h"
#include <QMimeData>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include "rapidjson/document.h"


MyQTextEdit::MyQTextEdit(QWidget* parent) :QTextEdit(parent) {}

void MyQTextEdit::dragEnterEvent(QDragEnterEvent* event)
{
    event->acceptProposedAction();
}

void MyQTextEdit::dragMoveEvent(QDragMoveEvent* event)
{
    event->acceptProposedAction();
}

// ��ק�����¼�
void MyQTextEdit::dropEvent(QDropEvent* event)
{
    const QMimeData* qm = event->mimeData();          // ��ȡMIMEData
    auto filePath = qm->urls()[0].toLocalFile();      // �ǻ�ȡ�϶��ļ��ı���·��
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::about(nullptr, nullptr, "can not open file!");
        return;
    }
    QTextStream in(&file);
    auto text = in.readAll();

    rapidjson::Document doc;
    doc.Parse(text.toStdString().c_str());
    if (doc.HasParseError())
    {
        QMessageBox::about(nullptr, nullptr, "fail to parse json");
        return;
    }
    emit inputFileSignal(text);
}
