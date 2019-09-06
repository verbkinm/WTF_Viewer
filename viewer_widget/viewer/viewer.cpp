#include <iostream>

#include <QMessageBox>
#include <QtMath>
#include <QMenu>

#include <QDebug>

#include "viewer.h"
#include "saver.h"

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
    _pCurrentScene->addText("Select file!");
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
        _spPixFilterPanel->setClusterRange(std::static_pointer_cast<Viewer_Clog_Processor>(_spViewerProcessor).get()->getClustersLengthVector());
        _spPixFilterPanel->setTotRange(std::static_pointer_cast<Viewer_Clog_Processor>(_spViewerProcessor).get()->getVectorOfLengthsOfTots());
        _spPixFilterPanel->setTotRangeFull(std::static_pointer_cast<Viewer_Clog_Processor>(_spViewerProcessor).get()->getVectorOfSumOfTots());
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
    setEnablePanels(true);
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
    else if(fileName.mid(fileName.length()-4,-1) == "clog")
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
        setEnablePanels(true);
        std::static_pointer_cast<Viewer_Clog_Processor>(_spViewerProcessor).get()->setFilter(createFilterFromPixFilterPanel());
        setImage(_spViewerProcessor->getImage());
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

    filter._clusterRangeBegin = _spPixFilterPanel->getClusterBegin();
    filter._clusterRangeEnd = _spPixFilterPanel->getClusterEnd();
    filter._totRangeBegin = _spPixFilterPanel->getTotBegin();
    filter._totRangeEnd = _spPixFilterPanel->getTotEnd();
    filter._totRangeBeginFull = _spPixFilterPanel->getTotBeginFull();
    filter._totRangeEndFull = _spPixFilterPanel->getTotEndFull();
    filter._isAllTotInCluster = _spPixFilterPanel->isAllTotInCluster();
    filter._isMidiPix = _spPixFilterPanel->isMediPix();
    filter._isFullTotRange = _spPixFilterPanel->isFullTotRange();

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
    std::static_pointer_cast<Viewer_Clog_Processor>(_spViewerProcessor).get()->setFilter(filter);
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
    ////        ui->angle->setValue(ui->angle->value() + 90);
    //    size_t matrix_rang = row;
    //    for (size_t i = 0; i < row / 2; i++)
    //    {
    //        for (size_t j = i; j < row - 1 - i; j++)
    //        {
    //            double tmp = arrayOrigin[i][j];
    //            arrayOrigin[i][j] = arrayOrigin[j][matrix_rang - i - 1];
    //            arrayOrigin[j][matrix_rang - i - 1] = arrayOrigin[matrix_rang - i - 1][matrix_rang - j - 1];
    //            arrayOrigin[matrix_rang - i - 1][matrix_rang - j - 1] = arrayOrigin[matrix_rang - j - 1][i];
    //            arrayOrigin[matrix_rang - j - 1][i] = tmp;
    //        }
    //    }
    //    slotRepaint();
}
void Viewer::slotRotateMinus()
{
    //    size_t matrix_rang = row;
    //    for (size_t i = 0; i < row / 2; i++)
    //    {
    //        for (size_t j = i; j < row - 1 - i; j++)
    //        {
    //            double tmp = arrayOrigin[i][j];
    //            arrayOrigin[i][j] = arrayOrigin[matrix_rang - j - 1][i];
    //            arrayOrigin[matrix_rang - j - 1][i] = arrayOrigin[matrix_rang - i - 1][matrix_rang - j - 1];
    //            arrayOrigin[matrix_rang - i - 1][matrix_rang - j - 1] = arrayOrigin[j][matrix_rang - i - 1];
    //            arrayOrigin[j][matrix_rang - i - 1] = tmp;
    //        }
    //    }
    //    //        ui->angle->setValue(ui->angle->value() - 90);
    //    slotRepaint();
}

void Viewer::slotMirrorHorizontal()
{
    //    for (size_t i = 0; i < row / 2; i++)
    //                std::swap(arrayOrigin[i], arrayOrigin[row - i - 1]);

    //    slotRepaint();
}
void Viewer::slotMirrorVertical()
{
    //    for (size_t i = 0; i < row; i++)
    //    {
    //        for (size_t j = 0; j < row / 2; j++)
    //                    std::swap(arrayOrigin[i][j], arrayOrigin[i][row - j - 1]);
    //    }

    //    slotRepaint();
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
