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

// 拖拽放下事件
void MyQTextEdit::dropEvent(QDropEvent* event)
{
    const QMimeData* qm = event->mimeData();          // 获取MIMEData
    auto filePath = qm->urls()[0].toLocalFile();      // 获取拖动文件的本地路径
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(nullptr, nullptr, "Can not open file!");
        return;
    }

    QTextStream in(&file);
    auto text = in.readAll();
    file.close();

    // 解析json
    rapidjson::Document doc;
    doc.Parse(text.toStdString().c_str());
    if (doc.HasParseError())
    {
        QMessageBox::critical(nullptr, nullptr, "Fail to Parse TID Json!");
        return;
    }
    this->setText(text);
    emit inputFileSignal(text);
}
