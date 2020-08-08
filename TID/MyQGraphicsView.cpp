#include "MyQGraphicsView.h"
#include "MyQGraphicsScene.h"
#include <QMessageBox>
#include "common.h"

MyQGraphicsView::MyQGraphicsView(QWidget* parent)
    :QGraphicsView(parent),
    scene(new MyQGraphicsScene(this)),
    pixItem(nullptr),
    item(nullptr),
    regionMode("lane")
{
    this->setStyleSheet("padding: 0px; border: 0px;");
    this->setScene(scene);
}

void MyQGraphicsView::onSliderChangeed(int value)
{
    if (cap.isOpened())
    {
        double pos = value / 1000.0 * frameNum;
        cap.set(CV_CAP_PROP_POS_FRAMES, pos);
        cap >> frame;
        setImage(frame);
    }
}

// 更新背景图片
void MyQGraphicsView::setImage(const cv::Mat& imgFrame)
{
    auto w = this->width();
    auto h = this->height();

    cv::Mat img;
    cv::resize(imgFrame, img, cv::Size(w, h));
    cv::cvtColor(img, img, CV_BGR2RGBA);//转换格式
    QImage qImg = QImage(img.data, img.cols, img.rows, img.step, QImage::Format_RGBA8888);

    scene->setSceneRect(0, 0, w, h);

    // 北京图片
    if (pixItem != nullptr)
    {
        pixItem->setPixmap(QPixmap::fromImage(qImg));
        pixItem->update();
    }
    else
    {
        pixItem = scene->addPixmap(QPixmap::fromImage(qImg));
    }

    // 参数轮廓
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

// 鼠标移动事件
void MyQGraphicsView::mouseMoveEvent(QMouseEvent* event)
{
    QPoint pt = toRelativePoint(event->pos(), this->size());
    ptCache = pt;
    emit mouseMoveSignal(pt);

    if (item != nullptr)
    {
        static_cast<MyQGraphicsItem*>(item)->updateParam(regionMode, contour, vecPointCache, ptCache);
        item->update();
    }
}

// 鼠标单击事件
void MyQGraphicsView::mousePressEvent(QMouseEvent* event)
{
    if (item == nullptr)
    {
        return;
    }

    QPoint point = event->pos();
    // 左键
    if (event->button() == Qt::LeftButton)
    {
        vecPointCache.append(toRelativePoint(point, this->size()));
        static_cast<MyQGraphicsItem*>(item)->updateParam(regionMode, contour, vecPointCache, ptCache);
        item->update();
    }
    // 右键
    else if (event->button() == Qt::RightButton)
    {
        if (vecPointCache.size() <= 3)
        {
            vecPointCache.clear();
            ptCache = QPoint(0, 0);
            static_cast<MyQGraphicsItem*>(item)->updateParam(regionMode, contour, vecPointCache, ptCache);
            item->update();
        }
        else
        {
            if (regionMode == "lane")
            {
                using T = std::pair<int, TIDLane>;
                const auto lane = std::max_element(contour.lanes.begin(), contour.lanes.end(), 
                    [](const T& a, const T& b) {return a.first < b.first; });
                int laneID = (lane == contour.lanes.end()) ? 0 : lane->first + 1;
                contour.lanes[laneID] = TIDLane();
                contour.lanes[laneID].pts = std::move(vecPointCache);
            }
            else if (regionMode == "region")
            {
                using T = std::pair<int, TIDRegion>;
                const auto region = std::max_element(contour.regions.begin(), contour.regions.end(),
                    [](const T& a, const T& b) {return a.first < b.first; });
                int regionID = (region == contour.regions.end()) ? 0 : region->first + 1;

                contour.regions[regionID] = TIDRegion();
                contour.regions[regionID].pts = std::move(vecPointCache);
            }
            vecPointCache.clear();
            ptCache = QPoint(0, 0);
            static_cast<MyQGraphicsItem*>(item)->updateParam(regionMode, contour, vecPointCache, ptCache);
            item->update();
        }
    }
}

// 缩放事件
void MyQGraphicsView::resizeEvent(QResizeEvent* event)
{
    if (!frame.empty())
    {
        setImage(frame);
        if (item != nullptr)
        {
            static_cast<MyQGraphicsItem*>(item)->updateParam(regionMode, contour, vecPointCache, ptCache);
            item->update();
        }
    }
}

void MyQGraphicsView::dragEnterEvent(QDragEnterEvent* event) 
{
    event->acceptProposedAction();
}

// 拖拽放下事件
void MyQGraphicsView::dropEvent(QDropEvent* event) 
{
    const QMimeData* qm = event->mimeData();          // 获取MIMEData
    filePath = qm->urls()[0].toLocalFile();           // 是获取拖动文件的本地路径
    emit dropFile(filePath);

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
    cap >> frame;

    setImage(frame);
}

// 设置绘图模式
void MyQGraphicsView::setRegionMode(const int mode)
{
    switch (mode)
    {
    case 0:
        regionMode = "lane";
        break;
    case 1:
        regionMode = "region";
        break;
    default:
        regionMode = "";
    }
}
