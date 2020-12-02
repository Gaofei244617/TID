#include "MyQGraphicsView.h"
#include "MyQGraphicsScene.h"
#include <QMessageBox>
#include "common.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QFileInfo>
#include <QDialog>

MyQGraphicsView::MyQGraphicsView(QWidget* parent)
    :QGraphicsView(parent),
    scene(new MyQGraphicsScene(this)),
    pixItem(nullptr),
    item(nullptr),
    frameNum(0),
    drawMode("BusLane"),
    cpt(nullptr),
    dragPointFlag(false)
{
    this->setStyleSheet("padding: 0px; border: 0px;");
    this->setScene(scene);
}

void MyQGraphicsView::onSliderChangeed(int value)
{
    if (cap.isOpened())
    {
        double val = value >= 1000 ? 999 : value;
        double pos = val / 1000.0 * frameNum;
        cap.set(CV_CAP_PROP_POS_FRAMES, pos);
        cap >> frame;
        setImage(frame);
    }
}

// ���±���ͼƬ
void MyQGraphicsView::setImage(const cv::Mat& imgFrame)
{
    auto w = this->width();
    auto h = this->height();

    cv::Mat img;
    cv::resize(imgFrame, img, cv::Size(w, h));
    cv::cvtColor(img, img, CV_BGR2RGBA);//ת����ʽ
    QImage qImg = QImage(img.data, img.cols, img.rows, img.step, QImage::Format_RGBA8888);

    scene->setSceneRect(0, 0, w, h);

    // ����ͼƬ
    if (pixItem != nullptr)
    {
        pixItem->setPixmap(QPixmap::fromImage(qImg));
        pixItem->update();
    }
    else
    {
        pixItem = scene->addPixmap(QPixmap::fromImage(qImg));
    }

    // ��������
    if (item != nullptr)
    {
        item->update();
    }
    else
    {
        item = new MyQGraphicsItem();
        scene->addItem(item);
        item->setPos(0, 0);
    }
}

// ����ƶ��¼�
void MyQGraphicsView::mouseMoveEvent(QMouseEvent* event)
{
    QPoint pt = toRelativePoint(event->pos(), this->size());
    int x = static_cast<int>(pt.x() / 10000.0 * frame.cols);
    int y = static_cast<int>(pt.y() / 10000.0 * frame.rows);
    QPoint pt2(x, y);

    // ��갴��״̬
    if (cpt != nullptr)
    {
        *cpt = pt;
        dragPointFlag = true;
    }
    ptCache = pt;
    emit mouseMoveSignal(pt, pt2);

    if (item != nullptr)
    {
        static_cast<MyQGraphicsItem*>(item)->updateParam(drawMode, m_contour, m_mesureData, vecPointCache, ptCache);
        item->update();
        emit updateJsonSignal(m_contour.toJsonString());
    }
}

// ��갴���¼�
void MyQGraphicsView::mousePressEvent(QMouseEvent* event)
{
    QPoint pt = toRelativePoint(event->pos(), this->size());
    
    QPoint* point = nullptr;
    double dist = 0xffff;

    std::tie(point, dist) = findPoint(m_contour, pt);
    if (dist < 100)
    {
        cpt = point;
    }
}

// ��굥���ͷ��¼�
void MyQGraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
    cpt = nullptr;
    if (item == nullptr)
    {
        return;
    }
    
    if (dragPointFlag)
    {
        dragPointFlag = false;
        return;
    }

    QPoint point = event->pos();
    // ���
    if (event->button() == Qt::LeftButton)
    {
        vecPointCache.append(toRelativePoint(point, this->size()));
        if (drawMode == "direction" && vecPointCache.size() == 2)
        {
            Direct directLine(vecPointCache[0], vecPointCache[1]);
            int id = getLaneID(directLine, m_contour);
            if (id != -1)
            {
                m_contour.lanes[id].direction = directLine;
            }
            vecPointCache.clear();
        }

        // ��������
        if (drawMode == "mesure")
        {
            m_mesureData.clear();
            if (vecPointCache.size() == 2)
            {
                m_mesureData = vecPointCache;
                vecPointCache.clear();

                double dx = (m_mesureData[0].x() - m_mesureData[1].x()) / 10000.0 * frame.cols;
                double dy = (m_mesureData[0].y() - m_mesureData[1].y()) / 10000.0 * frame.rows;
                int len = static_cast<int>(sqrt(dx * dx + dy * dy));
                QString s = QString::fromLocal8Bit("����: %1 px  ").arg(len);
                QMessageBox::about(nullptr, nullptr, s);
                //QDialog* dialog = new QDialog();
                //dialog->setAttribute(Qt::WA_DeleteOnClose);
                //dialog->setModal(true);
                //dialog->show();
            }
        }

        static_cast<MyQGraphicsItem*>(item)->updateParam(drawMode, m_contour, m_mesureData, vecPointCache, ptCache);
        item->update();
        emit updateJsonSignal(m_contour.toJsonString());
    }
    // �Ҽ�
    else if (event->button() == Qt::RightButton)
    {
        if (vecPointCache.size() <= 3)
        {
            vecPointCache.clear();
            ptCache = QPoint(0, 0);
            static_cast<MyQGraphicsItem*>(item)->updateParam(drawMode, m_contour, m_mesureData, vecPointCache, ptCache);
            item->update();
        }
        else
        {
            if (drawMode == "BusLane" || drawMode == "EmergencyLane")
            {
                using T = std::pair<int, TIDLane>;
                const auto lane = std::max_element(m_contour.lanes.begin(), m_contour.lanes.end(), 
                    [](const T& a, const T& b) {return a.first < b.first; });
                int laneID = (lane == m_contour.lanes.end()) ? 0 : lane->first + 1;
                m_contour.lanes[laneID] = TIDLane();
                m_contour.lanes[laneID].type = drawMode;
                m_contour.lanes[laneID].lane = std::move(vecPointCache);
                //if (m_contour.lanes[laneID].lane.size() != 4)
                //{
                //    QMessageBox::warning(nullptr, nullptr, QString::fromLocal8Bit("����������Ϊ4"));
                //}
            }
            else if (drawMode == "region")
            {
                using T = std::pair<int, TIDRegion>;
                const auto region = std::max_element(m_contour.regions.begin(), m_contour.regions.end(),
                    [](const T& a, const T& b) {return a.first < b.first; });
                int regionID = (region == m_contour.regions.end()) ? 0 : region->first + 1;

                m_contour.regions[regionID] = TIDRegion();
                m_contour.regions[regionID].region = std::move(vecPointCache);
            }
            else if (drawMode == "loop")
            {
                QPolygon loop(vecPointCache);
                int id = getLaneID(loop, m_contour);
                if (id != -1)
                {
                    m_contour.lanes[id].virtualLoop = std::move(loop);
                }
            }
            vecPointCache.clear();
            ptCache = QPoint(0, 0);
            static_cast<MyQGraphicsItem*>(item)->updateParam(drawMode, m_contour, m_mesureData, vecPointCache, ptCache);
            item->update();
            emit updateJsonSignal(m_contour.toJsonString());
        }
    }
}

// �����¼�
void MyQGraphicsView::resizeEvent(QResizeEvent* event)
{
    if (!frame.empty())
    {
        setImage(frame);
        if (item != nullptr)
        {
            static_cast<MyQGraphicsItem*>(item)->updateParam(drawMode, m_contour, m_mesureData, vecPointCache, ptCache);
            item->update();
        }
    }
}

void MyQGraphicsView::dragEnterEvent(QDragEnterEvent* event) 
{
    event->acceptProposedAction();
}

// ��ק�����¼�
void MyQGraphicsView::dropEvent(QDropEvent* event) 
{
    const QMimeData* qm = event->mimeData();          // ��ȡMIMEData
    filePath = qm->urls()[0].toLocalFile();           // �ǻ�ȡ�϶��ļ��ı���·��
    actionOnOpenFile(filePath);
}

void MyQGraphicsView::actionOnOpenFile(QString filePath)
{
    QFileInfo file_info(filePath);
    QString suffix = file_info.suffix();   // �ļ���׺��
    // Json�ļ�
    if (suffix == "json")
    {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QMessageBox::warning(nullptr, nullptr, "Can not open file\n" + filePath);
            return;
        }

        QTextStream in(&file);
        auto text = in.readAll();
        file.close();

        // ����json
        rapidjson::Document doc;
        doc.Parse(text.toStdString().c_str());
        if (doc.HasParseError())
        {
            QMessageBox::critical(nullptr, nullptr, "Fail to Parse TID Json!");
            return;
        }
        setContour(text);
        if (item != nullptr)
        {
            static_cast<MyQGraphicsItem*>(item)->updateParam(drawMode, m_contour, m_mesureData, vecPointCache, ptCache);
            item->update();  // ����paint��Ա����
            emit updateJsonSignal(m_contour.toJsonString());
        }
        return;
    }

    // ͼƬ�ļ�
    if (suffix == "jpg" || suffix == "bmp" || suffix == "png")
    {
        frame = cv::imread(filePath.toStdString());
        setImage(frame);

        QJsonObject info;
        cv::Size imgSize = frame.size();
        info.insert("Name", filePath);
        info.insert("FileType", "Image");
        info.insert("Width", imgSize.width);
        info.insert("Height", imgSize.height);
        emit openFileSignal(QJsonDocument(info).toJson(QJsonDocument::Compact));

        return;
    }

    // xml�ļ�
    if (suffix == "xml")
    {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QMessageBox::warning(nullptr, nullptr, "Can not open file\n" + filePath);
            return;
        }

        auto bndboxes = getBndBox(&file);
        file.close();
        drawBox(bndboxes, frame);
        setImage(frame);

        return;
    }

    // ��Ƶ�ļ�
    if (cap.isOpened())
    {
        cap.release();
    }

    cap.open(filePath.toStdString());
    if (!cap.isOpened())
    {
        QMessageBox::about(nullptr, nullptr, "fail to open file");
        return;
    }
    frameNum = cap.get(CV_CAP_PROP_FRAME_COUNT);
    auto fps = cap.get(CV_CAP_PROP_FPS);
    auto width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
    auto height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
    cap >> frame;
    setImage(frame);

    QJsonObject info;
    info.insert("Name", filePath);
    info.insert("FileType", "Video");
    info.insert("FrameCount", frameNum);
    info.insert("FPS", fps);
    info.insert("Width", width);
    info.insert("Height", height);
    info.insert("Time", round(frameNum / fps * 100.0 / 60.0) / 100.0);  // ��Ƶʱ��(min)
    emit openFileSignal(QJsonDocument(info).toJson(QJsonDocument::Compact));
}

void MyQGraphicsView::setContour(const QString& str)
{
    m_contour = getTIDContour(str);
    vecPointCache.clear();
    ptCache = QPoint(0, 0);
    if (item != nullptr)
    {
        static_cast<MyQGraphicsItem*>(item)->updateParam(drawMode, m_contour, m_mesureData, vecPointCache, ptCache);
        item->update();
    }
    else
    {
        QMessageBox::about(nullptr, nullptr, "Please open video first.");
    }
}

// ��ȡ��ǰ��Ƶ֡
cv::Mat MyQGraphicsView::getCurrentFrame()const
{
    return this->frame;
}

// ���û�ͼģʽ
void MyQGraphicsView::setRegionMode(const int mode)
{
    switch (mode)
    {
    case 0:
        drawMode = "BusLane";
        break;
    case 1:
        drawMode = "EmergencyLane";
        break;
    case 2:
        drawMode = "region";
        break;
    case 3:
        drawMode = "direction";
        break;
    case 4:
        drawMode = "loop";
        break;
    case 5:
        drawMode = "mesure";
        break;
    default:
        drawMode = "";
    }
}

// ��ջ�ͼ����
void MyQGraphicsView::clearContour()
{
    m_contour.lanes.clear();
    m_contour.regions.clear();
    vecPointCache.clear();
    ptCache = QPoint(0, 0);
    if (item != nullptr)
    {
        static_cast<MyQGraphicsItem*>(item)->updateParam(drawMode, m_contour, m_mesureData, vecPointCache, ptCache);
        item->update();
        emit updateJsonSignal(m_contour.toJsonString());
    }
}
