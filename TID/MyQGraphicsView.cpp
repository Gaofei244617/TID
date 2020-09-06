#include "MyQGraphicsView.h"
#include "MyQGraphicsScene.h"
#include <QMessageBox>
#include "common.h"
#include <QJsonDocument>
#include <QJsonObject>

MyQGraphicsView::MyQGraphicsView(QWidget* parent)
    :QGraphicsView(parent),
    scene(new MyQGraphicsScene(this)),
    pixItem(nullptr),
    item(nullptr),
    drawMode("BusLane")
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
    ptCache = pt;
    emit mouseMoveSignal(pt);

    if (item != nullptr)
    {
        static_cast<MyQGraphicsItem*>(item)->updateParam(drawMode, m_contour, vecPointCache, ptCache);
        item->update();
    }
}

// ��굥���¼�
void MyQGraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
    if (item == nullptr)
    {
        return;
    }

    QPoint point = event->pos();
    // ���
    if (event->button() == Qt::LeftButton)
    {
        vecPointCache.append(toRelativePoint(point, this->size()));
        if (drawMode == "direction" && vecPointCache.size() == 2)
        {
            QLine directLine = QLine(vecPointCache[0], vecPointCache[1]);
            int id = getLaneID(directLine, m_contour);
            if (id != -1)
            {
                m_contour.lanes[id].direction = std::move(directLine);
            }
            vecPointCache.clear();
        }
        static_cast<MyQGraphicsItem*>(item)->updateParam(drawMode, m_contour, vecPointCache, ptCache);
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
            static_cast<MyQGraphicsItem*>(item)->updateParam(drawMode, m_contour, vecPointCache, ptCache);
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
            static_cast<MyQGraphicsItem*>(item)->updateParam(drawMode, m_contour, vecPointCache, ptCache);
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
            static_cast<MyQGraphicsItem*>(item)->updateParam(drawMode, m_contour, vecPointCache, ptCache);
            item->update();
        }
    }
}

void MyQGraphicsView::dragEnterEvent(QDragEnterEvent* event) 
{
    event->acceptProposedAction();
}

void MyQGraphicsView::actionOnOpenFile(QString filePath)
{
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
    cap >> frame;
    setImage(frame);

    QJsonObject info;
    info.insert("Name", filePath);
    info.insert("FrameCount", frameNum);
    info.insert("FPS", fps);
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
        static_cast<MyQGraphicsItem*>(item)->updateParam(drawMode, m_contour, vecPointCache, ptCache);
        item->update();
    }
    else
    {
        QMessageBox::about(nullptr, nullptr, "Please open video first.");
    }
}

// ��ק�����¼�
void MyQGraphicsView::dropEvent(QDropEvent* event) 
{
    const QMimeData* qm = event->mimeData();          // ��ȡMIMEData
    filePath = qm->urls()[0].toLocalFile();           // �ǻ�ȡ�϶��ļ��ı���·��
    actionOnOpenFile(filePath);
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
        static_cast<MyQGraphicsItem*>(item)->updateParam(drawMode, m_contour, vecPointCache, ptCache);
        item->update();
        emit updateJsonSignal(m_contour.toJsonString());
    }
}
