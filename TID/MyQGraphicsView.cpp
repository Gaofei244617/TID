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
	m_scene(new MyQGraphicsScene(this)),
	m_pixItem(nullptr),
	m_item(nullptr),
	m_frameNum(0),
	m_drawMode("BusLane"),
	m_cpt(nullptr),
	m_dragPointFlag(false)
{
	this->setStyleSheet("padding: 0px; border: 0px;");
	this->setScene(m_scene);
}

void MyQGraphicsView::onSliderChangeed(int value)
{
	if (m_cap.isOpened())
	{
		double val = value >= 1000 ? 999 : value;
		double pos = val / 1000.0 * m_frameNum;
		m_cap.set(CV_CAP_PROP_POS_FRAMES, pos);
		m_cap >> m_frame;
		setImage(m_frame);
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

	m_scene->setSceneRect(0, 0, w, h);

	// ����ͼƬ
	if (m_pixItem != nullptr)
	{
		m_pixItem->setPixmap(QPixmap::fromImage(qImg));
		m_pixItem->update();
	}
	else
	{
		m_pixItem = m_scene->addPixmap(QPixmap::fromImage(qImg));
	}

	// ��������
	if (m_item != nullptr)
	{
		m_item->update();
	}
	else
	{
		m_item = new MyQGraphicsItem();
		m_scene->addItem(m_item);
		m_item->setPos(0, 0);
	}
}

// ����ƶ��¼�
void MyQGraphicsView::mouseMoveEvent(QMouseEvent* event)
{
	QPoint pt = toRelativePoint(event->pos(), this->size());
	int x = static_cast<int>(pt.x() / 10000.0 * m_frame.cols);
	int y = static_cast<int>(pt.y() / 10000.0 * m_frame.rows);
	QPoint pt2(x, y);

	// ��갴��״̬
	if (m_cpt != nullptr)
	{
		*m_cpt = pt;
		m_dragPointFlag = true;
	}
	m_ptCache = pt;
	emit mouseMoveSignal(pt, pt2);

	if (m_item != nullptr)
	{
		static_cast<MyQGraphicsItem*>(m_item)->updateParam(m_drawMode, m_contour, m_mesureData, m_vecPointCache, m_ptCache);
		m_item->update();
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
		m_cpt = point;
	}
}

// ��굥���ͷ��¼�
void MyQGraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
	m_cpt = nullptr;
	if (m_item == nullptr)
	{
		return;
	}

	if (m_dragPointFlag)
	{
		m_dragPointFlag = false;
		return;
	}

	QPoint point = toRelativePoint(event->pos(), this->size());
	// ���
	if (event->button() == Qt::LeftButton)
	{
		// �������β�պ�
		double dist = 0xffff;
		std::tie(std::ignore, dist) = findPoint(m_vecPointCache, point);
		if (dist < 200)
		{
			releaseOnRightBtn();
			return;
		}

		m_vecPointCache.append(point);
		// ��������
		if (m_drawMode == "direction" && m_vecPointCache.size() == 2)
		{
			Direct directLine(m_vecPointCache[0], m_vecPointCache[1]);
			int id = getLaneID(directLine, m_contour);
			if (id != -1)
			{
				m_contour.lanes[id].direction = directLine;
				if (m_contour.lanes[id].virtualLoop.size() == 0)
				{
					// �Զ�����������Ȧ
					m_contour.lanes[id].virtualLoop = calVirtualLoop(m_contour.lanes[id].lane, directLine);
				}
			}
			m_vecPointCache.clear();
		}

		// ��������
		if (m_drawMode == "mesure")
		{
			m_mesureData.clear();
			if (m_vecPointCache.size() == 2)
			{
				m_mesureData = m_vecPointCache;
				m_vecPointCache.clear();

				double dx = (m_mesureData[0].x() - m_mesureData[1].x()) / 10000.0 * m_frame.cols;
				double dy = (m_mesureData[0].y() - m_mesureData[1].y()) / 10000.0 * m_frame.rows;
				int len = static_cast<int>(sqrt(dx * dx + dy * dy));
				QString s = QString::fromLocal8Bit("����: %1 px  ").arg(len);
				QMessageBox::about(nullptr, nullptr, s);
				//QDialog* dialog = new QDialog();
				//dialog->setAttribute(Qt::WA_DeleteOnClose);
				//dialog->setModal(true);
				//dialog->show();
			}
		}

		static_cast<MyQGraphicsItem*>(m_item)->updateParam(m_drawMode, m_contour, m_mesureData, m_vecPointCache, m_ptCache);
		m_item->update();
		emit updateJsonSignal(m_contour.toJsonString());
	}
	// �Ҽ�
	else if (event->button() == Qt::RightButton)
	{
		releaseOnRightBtn();
	}
}

// �����¼�
void MyQGraphicsView::resizeEvent(QResizeEvent* event)
{
	if (!m_frame.empty())
	{
		setImage(m_frame);
		if (m_item != nullptr)
		{
			static_cast<MyQGraphicsItem*>(m_item)->updateParam(m_drawMode, m_contour, m_mesureData, m_vecPointCache, m_ptCache);
			m_item->update();
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
	m_filePath = qm->urls()[0].toLocalFile();           // �ǻ�ȡ�϶��ļ��ı���·��
	actionOnOpenFile(m_filePath);
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
		if (m_item != nullptr)
		{
			static_cast<MyQGraphicsItem*>(m_item)->updateParam(m_drawMode, m_contour, m_mesureData, m_vecPointCache, m_ptCache);
			m_item->update();  // ����paint��Ա����
			emit updateJsonSignal(m_contour.toJsonString());
		}
		return;
	}

	// ͼƬ�ļ�
	if (suffix == "jpg" || suffix == "bmp" || suffix == "png")
	{
		m_frame = cv::imread(filePath.toStdString());
		setImage(m_frame);

		QJsonObject info;
		cv::Size imgSize = m_frame.size();
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

		if (m_item != nullptr)
		{
			auto bndboxes = getBndBox(&file, QSize(m_frame.cols, m_frame.rows));
			static_cast<MyQGraphicsItem*>(m_item)->setObjBox(bndboxes);
			m_item->update();  // ����paint��Ա����
			emit updateJsonSignal(m_contour.toJsonString());
		}

		file.close();
		return;
	}

	// ��Ƶ�ļ�
	if (m_cap.isOpened())
	{
		m_cap.release();
	}

	m_cap.open(filePath.toStdString());
	if (!m_cap.isOpened())
	{
		QMessageBox::about(nullptr, nullptr, "fail to open file");
		return;
	}
	m_frameNum = m_cap.get(CV_CAP_PROP_FRAME_COUNT);
	auto fps = m_cap.get(CV_CAP_PROP_FPS);
	auto width = m_cap.get(CV_CAP_PROP_FRAME_WIDTH);
	auto height = m_cap.get(CV_CAP_PROP_FRAME_HEIGHT);
	m_cap >> m_frame;
	setImage(m_frame);

	QJsonObject info;
	info.insert("Name", filePath);
	info.insert("FileType", "Video");
	info.insert("FrameCount", m_frameNum);
	info.insert("FPS", fps);
	info.insert("Width", width);
	info.insert("Height", height);
	info.insert("Time", round(m_frameNum / fps * 100.0 / 60.0) / 100.0);  // ��Ƶʱ��(min)
	emit openFileSignal(QJsonDocument(info).toJson(QJsonDocument::Compact));
}

void MyQGraphicsView::setContour(const QString& str)
{
	m_contour = getTIDContour(str);
	m_vecPointCache.clear();
	m_ptCache = QPoint(0, 0);
	if (m_item != nullptr)
	{
		static_cast<MyQGraphicsItem*>(m_item)->updateParam(m_drawMode, m_contour, m_mesureData, m_vecPointCache, m_ptCache);
		m_item->update();
	}
	else
	{
		QMessageBox::about(nullptr, nullptr, "Please open video first.");
	}
}

// ��ȡ��ǰ��Ƶ֡
cv::Mat MyQGraphicsView::getCurrentFrame()const
{
	return this->m_frame;
}

// ���û�ͼģʽ
void MyQGraphicsView::setRegionMode(const int mode)
{
	switch (mode)
	{
	case 0:
		m_drawMode = "BusLane";
		break;
	case 1:
		m_drawMode = "EmergencyLane";
		break;
	case 2:
		m_drawMode = "region";
		break;
	case 3:
		m_drawMode = "direction";
		break;
	case 4:
		m_drawMode = "loop";
		break;
	case 5:
		m_drawMode = "mesure";
		break;
	default:
		m_drawMode = "";
	}
}

// ��ջ�ͼ����
void MyQGraphicsView::clearContour()
{
	m_contour.lanes.clear();
	m_contour.regions.clear();
	m_vecPointCache.clear();
	m_ptCache = QPoint(0, 0);
	if (m_item != nullptr)
	{
		static_cast<MyQGraphicsItem*>(m_item)->updateParam(m_drawMode, m_contour, m_mesureData, m_vecPointCache, m_ptCache);
		static_cast<MyQGraphicsItem*>(m_item)->setObjBox(QVector<BndBox>());
		m_item->update();
		emit updateJsonSignal(m_contour.toJsonString());
	}
}

// ����Ҽ��ͷ���Ӧ����
void MyQGraphicsView::releaseOnRightBtn()
{
	if (m_vecPointCache.size() <= 3)
	{
		m_vecPointCache.clear();
		m_ptCache = QPoint(0, 0);
		static_cast<MyQGraphicsItem*>(m_item)->updateParam(m_drawMode, m_contour, m_mesureData, m_vecPointCache, m_ptCache);
		m_item->update();
	}
	else
	{
		if (m_drawMode == "BusLane" || m_drawMode == "EmergencyLane")
		{
			using T = std::pair<int, TIDLane>;
			const auto lane = std::max_element(m_contour.lanes.begin(), m_contour.lanes.end(),
				[](const T& a, const T& b) {return a.first < b.first; });
			int laneID = (lane == m_contour.lanes.end()) ? 0 : lane->first + 1;
			m_contour.lanes[laneID] = TIDLane();
			m_contour.lanes[laneID].type = m_drawMode;
			m_contour.lanes[laneID].lane = std::move(m_vecPointCache);
			//if (m_contour.lanes[laneID].lane.size() != 4)
			//{
			//    QMessageBox::warning(nullptr, nullptr, QString::fromLocal8Bit("����������Ϊ4"));
			//}
		}
		else if (m_drawMode == "region")
		{
			using T = std::pair<int, TIDRegion>;
			const auto region = std::max_element(m_contour.regions.begin(), m_contour.regions.end(),
				[](const T& a, const T& b) {return a.first < b.first; });
			int regionID = (region == m_contour.regions.end()) ? 0 : region->first + 1;

			m_contour.regions[regionID] = TIDRegion();
			m_contour.regions[regionID].region = std::move(m_vecPointCache);
		}
		else if (m_drawMode == "loop")
		{
			QPolygon loop(m_vecPointCache);
			int id = getLaneID(loop, m_contour);
			if (id != -1)
			{
				m_contour.lanes[id].virtualLoop = std::move(loop);
			}
		}
		m_vecPointCache.clear();
		m_ptCache = QPoint(0, 0);
		static_cast<MyQGraphicsItem*>(m_item)->updateParam(m_drawMode, m_contour, m_mesureData, m_vecPointCache, m_ptCache);
		m_item->update();
		emit updateJsonSignal(m_contour.toJsonString());
	}
}