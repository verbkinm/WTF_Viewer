#include <iostream>
#include <cmath>

#include <QMessageBox>
#include <QtMath>
#include <QMenu>

#include "viewer.h"
#include "saver.h"
#include "../prefilter.h"

#include "ui_viewer.h"

Viewer::Viewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Viewer),
    _spViewerProcessor(nullptr),
    _spViewerButtonPanel(nullptr), _spViewerDataPanel(nullptr), _spPixFilterPanel(nullptr),
    _spSettings(nullptr),
    _readOnly(false),
    _itemImage(nullptr), _itemRect(nullptr),
    _pEventFilterScene(nullptr), _pEventFilterViewPort(nullptr),
    _spMenuFile(nullptr),
    _pCurrentScene(&_defaultScene)
{ 
    this->setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);

    _pCurrentScene->setObjectName("scene");     //установка сцены
    ui->graphicsView->setScene(_pCurrentScene);
    _pEventFilterScene = new FingerSlide;     //фильтр событий для сцены (перемещения выделения; x, y и data положения курсора над рисунком
    _pCurrentScene->installEventFilter(_pEventFilterScene);

    //представление
    ui->graphicsView->viewport()->setObjectName("viewport");
    _pEventFilterViewPort = new FingerSlide;     //фильтр событий для представления (колёсико мышы и перетаскивание рисунка мышью)
    ui->graphicsView->viewport()->installEventFilter(_pEventFilterViewPort);

    createButtonMenu();
    createDataPanel();
    createButtonPanel();
    createPixFilterPanel();

    connect(ui->inversion, SIGNAL(stateChanged(int)), this, SLOT(slotImageAccordingInversionCheckBox(int)));     //инверсия цвета
    incorrectFile();     //при первом запуске - вывести на экран надпись и отключить всё не нужное
}
void Viewer::setEmptyImageOnViewerScene()
{
    _pCurrentScene->clear();
    ui->graphicsView->resetTransform();
    _pCurrentScene->setSceneRect(QRect(0,0, 50,50));
    auto item = _pCurrentScene->addText("Select file!");
    item->setObjectName("incorrectFile");
}
void Viewer::setSceneReadOnly(bool state)
{
    _readOnly = state;
    if(_itemRect)
    {
        _itemRect->setFlag(QGraphicsItem::ItemIsMovable, !state);
        _itemRect->setFlag(QGraphicsItem::ItemIsSelectable, !state);
    }
    if(state)
        _pCurrentScene->setObjectName("NONE");
    else
        _pCurrentScene->setObjectName("scene");
}
void Viewer::hideAllPanel()
{
    for(int i = 0; i < _spMenuFile->actions().length() - 1; ++i)
        _spMenuFile->actions()[i]->trigger();
}
void Viewer::hideSettingsButton(bool state)
{
    _spMenuFile = nullptr;
    ui->button_settings->setVisible(!state);
    ui->marker_label->setVisible(!state);
}

std::vector<std::vector<double>> Viewer::getVec2D() const
{
    return _spViewerProcessor->getVec2D();
}

void Viewer::setEnablePanels(bool state)
{
    this->setEnabled(state);
    if(!_spPixFilterPanel)
        return;
    _spPixFilterPanel->reset(true);
    _spPixFilterPanel->setDataPanelEnabled(!state);
    if(!_spViewerProcessor)
        return;
    if(_spViewerProcessor->getFileType() == Viewer_Processor::fileType::TXT)
        _spPixFilterPanel->setTabEnable(Pix_Filter_Panel::CLOG_FILTER_TAB, false);
    else if(_spViewerProcessor->getFileType() == Viewer_Processor::fileType::CLOG)
    {
        _spPixFilterPanel->setTabEnable(Pix_Filter_Panel::CLOG_FILTER_TAB, true);
        _spPixFilterPanel->setClusterRange(std::static_pointer_cast<Viewer_Clog_Processor>(_spViewerProcessor).get()->getClusterRange());
        _spPixFilterPanel->setTotRange(std::static_pointer_cast<Viewer_Clog_Processor>(_spViewerProcessor).get()->getTotsRange());
        _spPixFilterPanel->setTotRangeFull(std::static_pointer_cast<Viewer_Clog_Processor>(_spViewerProcessor).get()->getTotsSumRange());
    }
}
void Viewer::setImage(const QImage &image)
{
    _currentImage = image;
    if(image.format() == QImage::Format_Invalid)
    {
        incorrectFile();
        return;
    }
    showMarkers();
    ui->graphicsView->resetTransform();
    _pCurrentScene->clear();
    _pCurrentScene->setSceneRect(image.rect());
    if(_spViewerDataPanel)
        _spViewerDataPanel->setData(0,0, 0.0, static_cast<size_t>(_pCurrentScene->width()), static_cast<size_t>(_pCurrentScene->height()) );
    ui->graphicsView->fitInView(image.rect(), Qt::KeepAspectRatio);
    _itemRect = nullptr;
    _itemImage = _pCurrentScene->addPixmap(QPixmap::fromImage(_currentImage));
    slotImageAccordingInversionCheckBox(ui->inversion->checkState());

    connect_pEventFilterScene();
}
void Viewer::setSettings(std::shared_ptr<const QSettings> spSettings)
{
    _spSettings = spSettings;
}
void Viewer::setScene(QGraphicsScene *scene)
{
    if(!scene)
        return;
    ui->graphicsView->setScene(scene);
    _pCurrentScene = scene;
}
void Viewer::setSceneDefault()
{
    _pCurrentScene = &_defaultScene;
    ui->graphicsView->setScene(_pCurrentScene);
}
void Viewer::setImageFileName(const QString &fileName)
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    _filePath = fileName;
    if(fileName.mid(fileName.length()-3,-1) == "txt")
    {
        _spViewerProcessor = std::make_shared<Viewer_Txt_Processor>();
        _spMenuFile->actions()[PIX_AND_FILTER_PANEL]->setEnabled(true);
        _spViewerProcessor->setSettings(_spSettings);
        _spViewerProcessor->setFileName(fileName);
        setImage(_spViewerProcessor->getImage());
    }
    else if(fileName.mid(fileName.length() -4, -1) == "clog")
    {
        if(!_spPixFilterPanel)
        {
            QApplication::restoreOverrideCursor();
            QMessageBox::critical(this, "Error", "Please, enable \"pix. & filter panel\"!");
            return;
        }
        _spViewerProcessor = std::make_shared<Viewer_Clog_Processor>();
        _spMenuFile->actions()[PIX_AND_FILTER_PANEL]->setDisabled(true);
        _spViewerProcessor->setSettings(_spSettings);
        _spViewerProcessor->setFileName(fileName);

        Filter_Clog filterClog;// = createFilterFromPixFilterPanel();
        const Frames &frames = std::static_pointer_cast<Viewer_Clog_Processor>(_spViewerProcessor).get()->getFrames();
        PreFilter prefilter(fileName, frames, this);
        QApplication::restoreOverrideCursor();
        if(prefilter.exec() == QDialog::Accepted)
        {
            filterClog._frameBegin = prefilter.getFrameMin();
            filterClog._frameEnd = prefilter.getFrameMax();
        }
        else
            _spViewerProcessor->clear();

        std::static_pointer_cast<Viewer_Clog_Processor>(_spViewerProcessor).get()->setFilter(filterClog);
        setEnablePanels(true);
        slotApplyClogFilter();
    }
    else
        incorrectFile();
    QApplication::restoreOverrideCursor();
}

void Viewer::showMarkers()
{
    QString border = "Border; ";
    QString mask = "Mask; ";
    QString generalCalibration = "General calibration; ";
    QString totMode = "Tot mode; ";

    switch (_spViewerProcessor->_markers.operator unsigned int())
    {
    case 0x0: ui->marker_label->clear();
        break;
    case 0x1: ui->marker_label->setText(border);
        break;
    case 0x2: ui->marker_label->setText(mask);
        break;
    case 0x3: ui->marker_label->setText(border + mask);
        break;
    case 0x4: ui->marker_label->setText(generalCalibration);
        break;
    case 0x5: ui->marker_label->setText(border + generalCalibration);
        break;
    case 0x6: ui->marker_label->setText(generalCalibration + mask);
        break;
    case 0x7: ui->marker_label->setText(border + mask + generalCalibration);
        break;
    case 0x8: ui->marker_label->setText(totMode);
        break;
    case 0x9: ui->marker_label->setText(totMode + border);
        break;
    case 10: ui->marker_label->setText(totMode + mask);
        break;
    case 11: ui->marker_label->setText(totMode + border + mask);
        break;
    default: ui->marker_label->setText("Invalid flag");
        break;
    }
}

void Viewer::createButtonMenu()
{
    _spMenuFile.reset(new QMenu);
    _spMenuFile->addAction("pix. && filter panel", this, SLOT(slotPixAndFilterPanelMenuToggle(bool)));
    _spMenuFile->addAction("data pnael", this, SLOT(slotDataPanelMenuToggle(bool)));
    _spMenuFile->addAction("buttons panel", this, SLOT(slotButtonPanelMenuToggle(bool)));
    _spMenuFile->addAction("inversion", this, SLOT(slotInversionMenuToggle(bool)));
    _spMenuFile->addSeparator();
    _spMenuFile->addAction("separate window", this, SLOT(slotSeparateWindowMenuToggle()));
    ui->button_settings->setMenu(_spMenuFile.get());
    ui->button_settings->setStyleSheet("QPushButton::menu-indicator { image: url(:/settings); top: -3px; left: -3px;}");

    int lenght = _spMenuFile->actions().length() - 1; // -1 - для последнего пункта не надо
    for (int i = 0; i < lenght; ++i)
    {
        _spMenuFile->actions()[i]->setCheckable(true);
        _spMenuFile->actions()[i]->setChecked(true);
    }
}

Filter_Clog Viewer::createFilterFromPixFilterPanel()
{
    Filter_Clog filter;
    if(!_spPixFilterPanel)
        return filter;

    filter._clusterRange.setMin(static_cast<size_t>(_spPixFilterPanel->getClusterBegin()));
    filter._clusterRange.setMax(static_cast<size_t>(_spPixFilterPanel->getClusterEnd()));
    filter._totRange.setMin(_spPixFilterPanel->getTotBegin());
    filter._totRange.setMax(_spPixFilterPanel->getTotEnd());
    filter._isAllTotInCluster = _spPixFilterPanel->isAllTotInCluster();
    filter._isMidiPix = _spPixFilterPanel->isMediPix();
    filter._isTotRangeChecked = _spPixFilterPanel->isTotRangeChecked();

    return filter;
}

void Viewer::createButtonPanel()
{
    _spViewerButtonPanel.reset(new Viewer_Button_Panel);
    connect(_spViewerButtonPanel.get(), SIGNAL(signalRotatePlus()), SLOT(slotRotatePlus()));
    connect(_spViewerButtonPanel.get(), SIGNAL(signalRotateMinus()), SLOT(slotRotateMinus()));
    connect(_spViewerButtonPanel.get(), SIGNAL(signalMirrorHorizontal()), SLOT(slotMirrorHorizontal()));
    connect(_spViewerButtonPanel.get(), SIGNAL(signalMirrorVertical()), SLOT(slotMirrorVertical()));
    connect(_spViewerButtonPanel.get(), SIGNAL(signalResetTransform()), SLOT(slotResetTransform()));
    connect(_spViewerButtonPanel.get(), SIGNAL(signalScaledPlus()), SLOT(slotScaledPlus()));
    connect(_spViewerButtonPanel.get(), SIGNAL(signalScaledMinus()), SLOT(slotScaledMinus()));
    connect(_spViewerButtonPanel.get(), SIGNAL(signalSaveToBmp()), SLOT(slotSaveBMP()));
    connect(_spViewerButtonPanel.get(), SIGNAL(signalSaveToTxt()), SLOT(slotSaveTXT()));
    this->layout()->addWidget(_spViewerButtonPanel.get());
}

void Viewer::createDataPanel()
{
    _spViewerDataPanel.reset(new Viewer_Data_Panel);
    if(_spViewerProcessor)
        _spViewerDataPanel->setData(0,0,0, _spViewerProcessor->getColumns(), _spViewerProcessor->getRows());
    if(_pEventFilterScene)
        connect(_pEventFilterScene, SIGNAL(signalMousePos(QPointF)), SLOT(slotViewPosition(QPointF)));
    this->layout()->addWidget(_spViewerDataPanel.get());
}

void Viewer::createPixFilterPanel()
{
    _spPixFilterPanel.reset(new Pix_Filter_Panel);
    ui->layout_graphicView_and_Pix_filter_panel->addWidget(_spPixFilterPanel.get());
    ui->layout_graphicView_and_Pix_filter_panel->setStretch(0,1);
    if(_spViewerProcessor && _spViewerProcessor->getFileType() == Viewer_Processor::fileType::TXT)
        _spPixFilterPanel->setTabEnable(Pix_Filter_Panel::CLOG_FILTER_TAB, false);
    //соединение сигналов высылаемых классом ToolsPanel
    connect(_spPixFilterPanel.get(), SIGNAL(signalSelectionToggle(bool)), SLOT(slotToggleFrame(bool)));
    connect(_spPixFilterPanel.get(), SIGNAL(signalPenToggle(bool)), SLOT(slotSetCursorAsPencil(bool)));
    connect(_spPixFilterPanel.get(), SIGNAL(signalCutClicked(bool)), SLOT(slotCut()));
    connect(_spPixFilterPanel.get(), SIGNAL(signalRepaint()), SLOT(slotRepaint()));
    connect(_spPixFilterPanel.get(), SIGNAL(signalDataOnDataPanelChanged(QRect)), SLOT(slotMoveRectFromKey(QRect)) );
    //Нажатия на кнопку Apply на панели фильтров clog
    connect(_spPixFilterPanel.get(), SIGNAL(signalApplyFilter()), this, SLOT(slotApplyClogFilter()));

    connect_pEventFilterScene();
}

void Viewer::slotSetImageFile(QString file)
{
    setImageFileName(file);
}
void Viewer::slotMoveRectFromKey(QRect rect)
{
    if(!_spPixFilterPanel || !_itemRect)
        return;
    _itemRect->setPos(0, 0);
    _itemRect->setRect(rect.x(), rect.y(), rect.width(), rect.height());
}
void Viewer::slotCreateRectItem(QGraphicsRectItem * item)
{
    _itemRect = item;
}
void Viewer::slotApplyClogFilter()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    Filter_Clog filter = createFilterFromPixFilterPanel();

    filter._frameBegin = std::static_pointer_cast<Viewer_Clog_Processor>(_spViewerProcessor)->getFrames()._filter._frameBegin;
    filter._frameEnd = std::static_pointer_cast<Viewer_Clog_Processor>(_spViewerProcessor)->getFrames()._filter._frameEnd;

    std::static_pointer_cast<Viewer_Clog_Processor>(_spViewerProcessor)->setFilter(filter);
    setImage(_spViewerProcessor->getImage());
    QApplication::restoreOverrideCursor();
}
void Viewer::slotRepaint()
{
    setImage(_spViewerProcessor->getRedrawnImage());
}

void Viewer::slotPixAndFilterPanelMenuToggle(bool state)
{
    if(state)
        createPixFilterPanel();
    else
    {
        if(_itemRect)
        {
            _pCurrentScene->removeItem(_itemRect);
            _itemRect = nullptr;
        }
        _spPixFilterPanel = nullptr;
        ui->graphicsView->unsetCursor();
    }
}
void Viewer::slotDataPanelMenuToggle(bool state)
{
    if(state)
        createDataPanel();
    else
        _spViewerDataPanel = nullptr;
}
void Viewer::slotButtonPanelMenuToggle(bool state)
{
    if(state)
        createButtonPanel();
    else
        _spViewerButtonPanel = nullptr;
}
void Viewer::slotInversionMenuToggle(bool state)
{
    ui->inversion->setVisible(state);
}
void Viewer::slotSeparateWindowMenuToggle()
{   
    Viewer* SW = new Viewer;
    SW->setWindowTitle(_filePath);
    SW->setSettings(_spSettings);
    SW->show();

    if(_spViewerProcessor  && _spViewerProcessor->getFileType() == Viewer_Processor::fileType::TXT)
        SW->setImageFileName(_filePath);
    else if(_spViewerProcessor  && _spViewerProcessor->getFileType() == Viewer_Processor::fileType::CLOG)
        SW->setImageFileName(_filePath);
}

void Viewer::slotViewSelectionMovePos(QPoint point)
{
    if(!_spPixFilterPanel)
        return;
    disconnect(_spPixFilterPanel.get(), SIGNAL(signalDataOnDataPanelChanged(QRect)), this, SLOT(slotMoveRectFromKey(QRect)));
    _spPixFilterPanel->setX(point.x());
    _spPixFilterPanel->setY(point.y());
    connect(_spPixFilterPanel.get(), SIGNAL(signalDataOnDataPanelChanged(QRect)), SLOT(slotMoveRectFromKey(QRect)) );
}

void Viewer::slotDrawPoint(QPointF point)
{
    if(!_spPixFilterPanel)
        return;

    int x = static_cast<int>(point.x());
    int y = static_cast<int>(point.y());

    if(x >= 0 && x < static_cast<int>(_spViewerProcessor->getColumns()) && y >= 0 && y < static_cast<int>(_spViewerProcessor->getRows()))
    {
        _currentImage.setPixelColor(x, y, _spPixFilterPanel->getPenColor());
        slotImageAccordingInversionCheckBox(ui->inversion->checkState());
        _spViewerProcessor->setDataInVec2D(static_cast<size_t>(x), static_cast<size_t>(y), _spPixFilterPanel->getPenValue());
    }
}
void Viewer::slotFinishSelection()
{
    ui->graphicsView->unsetCursor();
    _spPixFilterPanel->setDataPanelReadOnly(false);
    _spPixFilterPanel->finishSelection();
    _spPixFilterPanel->setButtonCutDisable(false);
}

void Viewer::incorrectFile()
{
    setEnablePanels(false);
    setEmptyImageOnViewerScene();

    if(_spPixFilterPanel)
        disconnect(_pEventFilterScene, SIGNAL(siganlRect(QRect)), _spPixFilterPanel.get(), SLOT(slotSetDataOnDataPanel(QRect)));
    //Отображение на панели координат выделения при перемещении(x,y)
    disconnect(_pEventFilterScene, SIGNAL(signalRectMove(QPoint)), this, SLOT(slotViewSelectionMovePos(QPoint)));
    //Изображение курсора - стрелка, выключение кнопки edit
    disconnect(_pEventFilterScene, SIGNAL(signalRelease()), this, SLOT(slotFinishSelection()));
    disconnect(_pEventFilterScene, SIGNAL(signalCreateRectItem(QGraphicsRectItem*)), this, SLOT(slotCreateRectItem(QGraphicsRectItem*)));
    disconnect(_pEventFilterScene, SIGNAL(signalDrawPoint(QPointF)), this, SLOT(slotDrawPoint(QPointF)));

    if(_spViewerDataPanel)
        _spViewerDataPanel->setData(0,0,0.0,0,0);
}

void Viewer::resetTransform()
{
    ui->graphicsView->resetTransform();
}

void Viewer::connect_pEventFilterScene()
{
    if(_spPixFilterPanel)
        connect(_pEventFilterScene, SIGNAL(siganlRect(QRect)), _spPixFilterPanel.get(), SLOT(slotSetDataOnDataPanel(QRect)), Qt::UniqueConnection);
    connect(_pEventFilterScene, SIGNAL(signalRectMove(QPoint)), this, SLOT(slotViewSelectionMovePos(QPoint)), Qt::UniqueConnection);
    connect(_pEventFilterScene, SIGNAL(signalRelease()), this, SLOT(slotFinishSelection()), Qt::UniqueConnection);
    connect(_pEventFilterScene, SIGNAL(signalCreateRectItem(QGraphicsRectItem*)), this, SLOT(slotCreateRectItem(QGraphicsRectItem*)), Qt::UniqueConnection);
    connect(_pEventFilterScene, SIGNAL(signalDrawPoint(QPointF)), this, SLOT(slotDrawPoint(QPointF)), Qt::UniqueConnection);
    connect(_pEventFilterViewPort, SIGNAL(signalWheel(int)), SLOT(slotScaleWheel(int)), Qt::UniqueConnection);
}

QGraphicsScene *Viewer::getScenePtr()
{
    if(_pCurrentScene)
        return _pCurrentScene;
    else
        return &_defaultScene;
}

std::pair<const Frames &, bool> Viewer::getFrames() const
{
    if(_spViewerProcessor->getFileType() != Viewer_Processor::fileType::CLOG)
        return {nullptr, false};
    return {std::static_pointer_cast<Viewer_Clog_Processor>(_spViewerProcessor).get()->getFrames(), true};
}
void Viewer::slotScaleWheel(int value)
{
    if(value > 0)
        ui->graphicsView->scale(1.1, 1.1);
    else if(value < 0)
        ui->graphicsView->scale(1 / 1.1, 1 / 1.1);
}
void Viewer::slotToggleFrame(bool state)
{
    if(state)
    {
        if(_itemRect)
        {
            _pCurrentScene->removeItem(_itemRect);
            _itemRect = nullptr;
        }
        _spPixFilterPanel->reset(state);
        _spPixFilterPanel->setDataPanelEnabled(state);
        _spPixFilterPanel->setDataPanelReadOnly(state);
        ui->graphicsView->setCursor(QCursor(Qt::CrossCursor));
    }
    if(!state && _itemRect == nullptr)
    {
        ui->graphicsView->unsetCursor();
        _spPixFilterPanel->setDataPanelEnabled(state);
        _spPixFilterPanel->setDataPanelReadOnly(!state);
    }
}

void Viewer::slotSetCursorAsPencil(bool value)
{
    if(value)
        ui->graphicsView->setCursor(QCursor(QPixmap(":/pen").scaled(24,24, Qt::KeepAspectRatio, Qt::SmoothTransformation), X_HOT ,Y_HOT));
    else
        ui->graphicsView->unsetCursor();
}
void Viewer::slotImageAccordingInversionCheckBox(int state)
{
    switch (state)
    {
    case (Qt::Unchecked):
        _itemImage->setPixmap(QPixmap::fromImage(_currentImage));
        break;
    case (Qt::Checked):
        QImage inversion(_currentImage);
        inversion.invertPixels(QImage::InvertRgb);
        _itemImage->setPixmap(QPixmap::fromImage(inversion));
        break;
    }
}
void Viewer::slotCut()
{
    if(!_itemRect)
    {
        QMessageBox::critical(this, "Error", "Please, select an area!");
        return;
    }
    if(!_spPixFilterPanel)
        return;
    std::vector<std::vector<double>> newVec2D = _spViewerProcessor->cutVec2D(static_cast<size_t>(_spPixFilterPanel->getX()),
                                                                             static_cast<size_t>(_spPixFilterPanel->getY()),
                                                                             static_cast<size_t>(_spPixFilterPanel->getWidth()),
                                                                             static_cast<size_t>(_spPixFilterPanel->getHeight()));
    QString fileName = Saver::saveInTemporaryTXT(static_cast<size_t>(_spPixFilterPanel->getWidth()),
                                                 static_cast<size_t>(_spPixFilterPanel->getHeight()),
                                                 newVec2D);
    setImageFileName(fileName);
}

void Viewer::slotRotatePlus()
{
    int clockwise = -1;
    double angle = _spViewerButtonPanel.get()->getAngle();      //угол поворота в градусах, полученный из формы
    if (angle != 0)
    {

        double radAngle = angle / 180. * M_PI, centrAngle;           //угол поворота в радианах. centrAngle - угол наклона отрезка
        //проходящего через точки центров нового и исходного изображения к оси OX

        int sourceWidth = _spViewerProcessor->getRows();       //горизонтальный размер исходной картинки
        int sourceHeight = _spViewerProcessor->getColumns();    //вертикальный размер исходной картинки
        int newWidth =  sourceWidth * fabs(cos(radAngle)) + sourceHeight * fabs(cos(M_PI/2-radAngle));          //горизонтальный размер новой картинки
        int newHeight = sourceWidth * fabs(sin(radAngle)) + sourceHeight * fabs(sin(M_PI/2-radAngle));       //вертикальный размер новой картинки

        //    qDebug()<<sourceWidth<<"  "<<sourceHeight<<"  "<<newWidth<<"  "<<newHeight;
        int hwn = newWidth /2,          //значения полувысот и полуширин нового и старого изображения
                hws = sourceWidth / 2,
                hhn = newHeight / 2,
                hhs = sourceHeight / 2;

        int radius = sqrt(hws*hws + hhs*hhs); //радиус рабочей области исходного изображения

        centrAngle = atan2(1. * hhs, hws);

        std::vector<std::vector<double>> newArrayOrigin(newWidth, std::vector<double>(newHeight));     //создание массива нового изображения

        for (int i = 0; i < newWidth; i++)       //обнуление массива нового изображения
        {
            for (int j = 0; j < newHeight; j++)  //
            {
                newArrayOrigin.at(i).at(j) = 0;     //
            }
        }

        for (int i = 0 - hwn; i < newWidth - hwn; i++)       //поворот
        {
            for (int j = 0 - hhn ; j < newHeight - hhn; j++)
            {
                int newI =             i * cos (radAngle) - clockwise * j * sin(radAngle) + radius * cos(centrAngle);
                int newJ = clockwise * i * sin (radAngle) +             j * cos(radAngle) + radius * sin(centrAngle);

                if ((newI < 2*hws)  && (newI >= 0) && (newJ < 2*hhs) && (newJ >= 0))    //проверка выхода за пределеы картинки
                    newArrayOrigin.at(i+hwn).at(j+hhn) = _spViewerProcessor->getVec2D().at(newI).at(newJ);  // рисуем итоговое изображение
                else newArrayOrigin.at(i+hwn).at(j+hhn) = 0;

            }
        }
        for (int i = 0 ; i < newWidth ; i++)       //прорисовка
        {
            for (int j = 0 ; j < newHeight ; j++)
            {
                _spViewerProcessor->setDataInVec2D(i,j,newArrayOrigin.at(i).at(j));
            }
        }

        //qDebug()<<newArrayOrigin.size();
        //qDebug()<<newArrayOrigin[0][255] << "  "<<newArrayOrigin[255][255];

        slotRepaint();
    }
}
void Viewer::slotRotateMinus()
{
    int conterclockwise = 1;
    double angle = _spViewerButtonPanel.get()->getAngle();      //угол поворота в градусах, полученный из формы
    if (angle != 0)
    {
        double radAngle = angle / 180. * M_PI, centrAngle;           //угол поворота в радианах. centrAngle - угол наклона отрезка
        //проходящего через точки центров нового и исходного изображения к оси OX

        int sourceWidth = _spViewerProcessor->getRows();       //горизонтальный размер исходной картинки
        int sourceHeight = _spViewerProcessor->getColumns();    //вертикальный размер исходной картинки
        int newWidth =  sourceWidth * fabs(cos(radAngle)) + sourceHeight * fabs(cos(M_PI/2-radAngle));          //горизонтальный размер новой картинки
        int newHeight = sourceWidth * fabs(sin(radAngle)) + sourceHeight * fabs(sin(M_PI/2-radAngle));       //вертикальный размер новой картинки

        //    qDebug()<<sourceWidth<<"  "<<sourceHeight<<"  "<<newWidth<<"  "<<newHeight;
        int hwn = newWidth /2,          //значения полувысот и полуширин нового и старого изображения
                hws = sourceWidth / 2,
                hhn = newHeight / 2,
                hhs = sourceHeight / 2;

        int radius = sqrt(hws*hws + hhs*hhs); //радиус рабочей области исходного изображения

        centrAngle = atan2(1. * hhs, hws);

        std::vector<std::vector<double>> newArrayOrigin(newWidth, std::vector<double>(newHeight));     //создание массива нового изображения

        for (int i = 0; i < newWidth; i++)       //обнуление массива нового изображения
        {
            for (int j = 0; j < newHeight; j++)  //
            {
                newArrayOrigin.at(i).at(j) = 0;     //
            }
        }

        for (int i = 0 - hwn; i < newWidth - hwn; i++)       //поворот
        {
            for (int j = 0 - hhn ; j < newHeight - hhn; j++)
            {
                int newI =             i * cos (radAngle) - conterclockwise * j * sin(radAngle) + radius * cos(centrAngle);
                int newJ = conterclockwise * i * sin (radAngle) +             j * cos(radAngle) + radius * sin(centrAngle);

                if ((newI < 2*hws)  && (newI >= 0) && (newJ < 2*hhs) && (newJ >= 0))    //проверка выхода за пределеы картинки
                    newArrayOrigin.at(i+hwn).at(j+hhn) = _spViewerProcessor->getVec2D().at(newI).at(newJ);  // рисуем итоговое изображение
                else newArrayOrigin.at(i+hwn).at(j+hhn) = 0;

            }
        }

        for (int i = 0 ; i < newWidth ; i++)       //прорисовка
        {
            for (int j = 0 ; j < newHeight ; j++)
            {
                _spViewerProcessor->setDataInVec2D(i,j,newArrayOrigin.at(i).at(j));
            }
        }
        //qDebug()<<newArrayOrigin.size();
        //qDebug()<<newArrayOrigin[0][255] << "  "<<newArrayOrigin[255][255];

        slotRepaint();
    }

}

void Viewer::slotMirrorHorizontal()
{

    int sourceWidth = _spViewerProcessor->getRows();       //горизонтальный размер исходной картинки
    int sourceHeight = _spViewerProcessor->getColumns();    //вертикальный размер исходной картинки
    std::vector<std::vector<double>> newArrayOrigin(sourceWidth, std::vector<double>(sourceHeight));     //создание массива нового изображения

    for (int i = 0; i < sourceWidth; i++)
        for(int j = 0; j < sourceWidth; j++)
        {
            newArrayOrigin.at(i).at(j) = _spViewerProcessor->getVec2D().at(i).at(j);
        }


    for (int i = 0; i < sourceWidth / 2; i++)
    {
        std::swap(newArrayOrigin.at(i), newArrayOrigin.at(sourceWidth - i - 1));
    }

    for (int i = 0 ; i < sourceWidth ; i++)       //прорисовка
    {
        for (int j = 0 ; j < sourceHeight ; j++)
        {
            _spViewerProcessor->setDataInVec2D(i,j,newArrayOrigin.at(i).at(j));
        }
    }

    slotRepaint();
}
void Viewer::slotMirrorVertical()
{
    int sourceWidth = _spViewerProcessor->getRows();       //горизонтальный размер исходной картинки
    int sourceHeight = _spViewerProcessor->getColumns();    //вертикальный размер исходной картинки
    std::vector<std::vector<double>> newArrayOrigin(sourceWidth, std::vector<double>(sourceHeight));     //создание массива нового изображения

    for (int i = 0; i < sourceWidth; i++)
        for(int j = 0; j < sourceWidth; j++)
        {
            newArrayOrigin.at(i).at(j) = _spViewerProcessor->getVec2D().at(i).at(j);
        }

    for (int i = 0; i < sourceWidth; i++)
    {
        for (int j = 0; j < sourceHeight / 2; j++)
            std::swap(newArrayOrigin[i][j], newArrayOrigin[i][sourceHeight - j - 1]);
    }

    for (int i = 0 ; i < sourceWidth ; i++)       //прорисовка
    {
        for (int j = 0 ; j < sourceHeight ; j++)
        {
            _spViewerProcessor->setDataInVec2D(i,j,newArrayOrigin.at(i).at(j));
        }
    }

    slotRepaint();
}
void Viewer::slotResetTransform()
{
    ui->graphicsView->resetTransform();
    ui->graphicsView->fitInView(_currentImage.rect(), Qt::KeepAspectRatio);
}
void Viewer::slotViewPosition(QPointF pos)
{
    if(!_spViewerDataPanel || !_spViewerProcessor)
        return;
    _spViewerDataPanel->setData(static_cast<size_t>(pos.x()),
                                static_cast<size_t>(pos.y()),
                                _spViewerProcessor->getDataInVec2D(static_cast<size_t>(pos.x()), static_cast<size_t>(pos.y())),
                                static_cast<size_t>(_pCurrentScene->sceneRect().width()),
                                static_cast<size_t>(_pCurrentScene->sceneRect().height()));
}
void Viewer::slotScaledPlus()
{
    ui->graphicsView->scale(1.1, 1.1);
}
void Viewer::slotScaledMinus()
{
    ui->graphicsView->scale(1 / 1.1, 1 / 1.1);
}
void Viewer::slotSaveBMP()
{
    QImage image = _currentImage;
    if(ui->inversion->isChecked())
        image.invertPixels();
    Saver::dialogSaveBMP(image);
}

void Viewer::slotSaveTXT()
{
    if(!_spViewerProcessor)
        return;
    Saver::dialogSaveTXT(_spViewerProcessor->getColumns(), _spViewerProcessor->getRows(), _spViewerProcessor->getVec2D());
}
Viewer::~Viewer()
{
    delete ui;
    delete _pEventFilterScene;
    delete _pEventFilterViewPort;

}
