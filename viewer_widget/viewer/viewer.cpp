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
    _pEventFilterScene(nullptr),
    _spMenuFile(nullptr),
    _pCurrentScene(&_defaultScene)
{ 
    this->setAttribute(Qt::WA_DeleteOnClose);

    for (size_t i = 0; i < _state_of_the_menu_items.size(); ++i)
         _state_of_the_menu_items[i] = true;

    ui->setupUi(this);

    createButtonMenu();
    createDataPanel();
    createButtonPanel();
    createPixFilterPanel();

    //инверсия цвета
    connect(ui->inversion, SIGNAL(stateChanged(int)), this, SLOT(slotInversionCheckBox(int)));

    //установка сцены
    _pCurrentScene->setObjectName("scene");
    ui->graphicsView->setScene(_pCurrentScene);
    //фильтр событий для сцены
    _pEventFilterScene = new FingerSlide(_pCurrentScene);
    _pCurrentScene->installEventFilter(_pEventFilterScene);

    //представление
    ui->graphicsView->viewport()->setObjectName("viewport");

    // !!!!!!!!!!!!!!!!!!!!!!!!!
    //фильтр событий для представления
    FingerSlide * eventFilterViewPort = new FingerSlide(ui->graphicsView->viewport());
    ui->graphicsView->viewport()->installEventFilter(eventFilterViewPort);
    connect(eventFilterViewPort, SIGNAL(signalWheel(int)), SLOT(slotScaleWheel(int)));

    //при первом запуске - вывести на экран надпись и отключить всё не нужное
    incorrectFile();
}
void Viewer::setEmptyImageOnViewerScene()
{
    _pCurrentScene->clear();
    ui->graphicsView->resetTransform();
    _pCurrentScene->setSceneRect(QRect(0,0, 50,50));
    _pCurrentScene->addText("Select file!");
}
void Viewer::setReadOnly(bool state)
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
    for(size_t i = 0; i < _state_of_the_menu_items.size(); ++i)
        _spMenuFile->actions()[static_cast<int>(i)]->trigger();
}
void Viewer::hideSettingsButton(bool state)
{
    _spMenuFile = nullptr;
    ui->button_settings->setVisible(!state);
    ui->marker_label->setVisible(!state);
}

void Viewer::setEnableDataPanelSelection(bool state)
{
//    disconnectPixFilterPanelSelectionBox();
    if(_spPixFilterPanel)
    {
        _spPixFilterPanel->setDataPanelEnabled(state);
        _spPixFilterPanel->setX(0);
        _spPixFilterPanel->setY(0);
        _spPixFilterPanel->setWidth(0);
        _spPixFilterPanel->setHeight(0);
    }
    setReadOnlyDataPanelSelection(state);
    connectPixFilterPanelSelectionBox();
}
void Viewer::setReadOnlyDataPanelSelection(bool state)
{
    if(_spPixFilterPanel)
        _spPixFilterPanel->setDataPanelReadOnly(state);
}
void Viewer::setEnableButtonPanel(bool state)
{
    if(_spViewerButtonPanel )
        _spViewerButtonPanel->setEnabled(state);
    if(_spViewerDataPanel)
        _spViewerDataPanel->setEnabled(state);

    ui->button_settings->setEnabled(state);
    ui->inversion->setEnabled(state);

    if(_spPixFilterPanel)
        _spPixFilterPanel->setEnabled(state);
    if(_spViewerProcessor  && _spViewerProcessor->getFileType() == Viewer_Processor::fileType::TXT && _spPixFilterPanel)
        _spPixFilterPanel->setTabEnable(Pix_Filter_Panel::CLOG_FILTER_TAB, false);
    else if(_spViewerProcessor  && _spViewerProcessor->getFileType() == Viewer_Processor::fileType::CLOG && _spPixFilterPanel)
    {
        _spPixFilterPanel->setTabEnable(Pix_Filter_Panel::CLOG_FILTER_TAB, true);
        _spPixFilterPanel->setClusterRange(std::static_pointer_cast<Viewer_Clog_Processor>(_spViewerProcessor).get()->getClustersLengthVector());
        _spPixFilterPanel->setTotRange(std::static_pointer_cast<Viewer_Clog_Processor>(_spViewerProcessor).get()->getVectorOfLengthsOfTots());
    }
    setEnableDataPanelSelection(false);
}
void Viewer::setImage(const QImage &image)
{
    _currentImage = image;
    if(image.format() != QImage::Format_Invalid)
    {
        setEnableButtonPanel(true);
        showMarkers();
        ui->graphicsView->resetTransform();
        _pCurrentScene->setSceneRect(image.rect());
        if(_spViewerDataPanel)
        {
            _spViewerDataPanel->setData(0,0, 0.0, static_cast<size_t>(_pCurrentScene->width()), static_cast<size_t>(_pCurrentScene->height()) );
            //Отображение на панели координаты курсора (x,y) относительно graphicsView
            connect(_pEventFilterScene, SIGNAL(signalMousePos(QPointF)), this, SLOT(slotViewPosition(QPointF)));
        }
        ui->graphicsView->fitInView(image.rect(), Qt::KeepAspectRatio);
        _pCurrentScene->clear();
        _itemRect = nullptr;
        _itemImage = _pCurrentScene->addPixmap(QPixmap::fromImage(_currentImage));
        slotInversionCheckBox(ui->inversion->checkState());
        connectEventFilter();
    }
    else
        incorrectFile();
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
void Viewer::setImageFile(const QString &fileName)
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    _filePath = fileName;
    if(fileName.mid(fileName.length()-3,-1) == "txt")
    {
        _spViewerProcessor = std::make_shared<Viewer_Txt_Processor>();

        _spMenuFile->actions()[PIX_AND_FILTER_PANEL]->setDisabled(false);
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
        setEnableButtonPanel(true);

        std::static_pointer_cast<Viewer_Clog_Processor>(_spViewerProcessor).get()->setFilter(createFilterFromPixFilterPanel());
        setImage(_spViewerProcessor->getImage());
    }
    else
        incorrectFile();
    QApplication::restoreOverrideCursor();
}
void Viewer::disconnectPixFilterPanelSelectionBox()
{
    if(!_spPixFilterPanel)
        return;
    disconnect(_spPixFilterPanel.get(), SIGNAL(signalValueX_Changed(int)), this, SLOT(slotMoveRectFromKey()) );
    disconnect(_spPixFilterPanel.get(), SIGNAL(signalValueY_Changed(int)), this, SLOT(slotMoveRectFromKey()) );
    disconnect(_spPixFilterPanel.get(), SIGNAL(signalValueWidth_Changed(int)), this, SLOT(slotMoveRectFromKey()) );
    disconnect(_spPixFilterPanel.get(), SIGNAL(signalValueHeight_Changed(int)), this, SLOT(slotMoveRectFromKey()) );
}

void Viewer::connectPixFilterPanel()
{
    //соединение сигналов высылаемых классом ToolsPanel
    connect(_spPixFilterPanel.get(), SIGNAL(signalSelectionToggle(bool)), this, SLOT(slotSelectionFrame(bool)));
    connect(_spPixFilterPanel.get(), SIGNAL(signalPenToggle(bool)), this, SLOT(slotPen(bool)));
    connect(_spPixFilterPanel.get(), SIGNAL(signalCutClicked(bool)), this, SLOT(slotCut()));
    connect(_spPixFilterPanel.get(), SIGNAL(signalRepaint()), this, SLOT(slotRepaint()));
    connectPixFilterPanelSelectionBox();
//    //Нажатия на кнопку Apply на панели фильтров clog
    connect(_spPixFilterPanel.get(), SIGNAL(signalApplyFilter()), this, SLOT(slotApplyClogFilter()));
}

void Viewer::connectPixFilterPanelSelectionBox()
{
    if(!_spPixFilterPanel)
        return;
    //изменение выделения с помощью спинбоксов
    connect(_spPixFilterPanel.get(), SIGNAL(signalValueX_Changed(int)), this, SLOT(slotMoveRectFromKey()) );
    connect(_spPixFilterPanel.get(), SIGNAL(signalValueY_Changed(int)), this, SLOT(slotMoveRectFromKey()) );
    connect(_spPixFilterPanel.get(), SIGNAL(signalValueWidth_Changed(int)), this, SLOT(slotMoveRectFromKey()) );
    connect(_spPixFilterPanel.get(), SIGNAL(signalValueHeight_Changed(int)), this, SLOT(slotMoveRectFromKey()) );
}

void Viewer::connectEventFilter()
{
    //Отображение на панели данных о выделении(x,y,width,height)
    connect(_pEventFilterScene, SIGNAL(siganlRect(QRect)), this, SLOT(slotViewSelectionPos(QRect)));
    //Отображение на панели координат выделения при перемещении(x,y)
    connect(_pEventFilterScene, SIGNAL(signalRectMove(QPoint)), this, SLOT(slotViewSelectionMovePos(QPoint)));
    //Изображение курсора - стрелка, выключение кнопки selection_button
    connect(_pEventFilterScene, SIGNAL(signalRelease()), this, SLOT(slotFinishSelection()));

    connect(_pEventFilterScene, SIGNAL(signalCreateRectItem(QGraphicsRectItem*)), this, SLOT(slotCreateRectItem(QGraphicsRectItem*)));
    connect(_pEventFilterScene, SIGNAL(signalDrawPoint(QPointF)), this, SLOT(slotDrawPoint(QPointF)));
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
    _spMenuFile->addAction("pix. && filter panel", this, SLOT(slotPixAndFilterPanelMenuToggle()));
    _spMenuFile->addAction("data pnael", this, SLOT(slotDataPanelMenuToggle()));
    _spMenuFile->addAction("buttons panel", this, SLOT(slotButtonPanelMenuToggle()));
    _spMenuFile->addAction("inversion", this, SLOT(slotInversionMenuToggle()));
    _spMenuFile->addSeparator();
    _spMenuFile->addAction("separate window", this, SLOT(slotSeparateWindowMenuToggle()));
    ui->button_settings->setMenu(_spMenuFile.get());
    ui->button_settings->setStyleSheet("QPushButton::menu-indicator { \
                                        image: url(:/settings); \
                                        top: -3px; \
                                        left: -3px;}");
/* Actions  [0 - pix. && filter pnael]
            [1 - data pnael]
            [2 - buttons panel]
            [3 - inversion]
            [4 - separate window]
  */
    size_t lenght = _state_of_the_menu_items.size() - 1; // -1 - для последнего пункта не надо
    for (unsigned int i = 0; i < lenght; ++i)
    {
        _spMenuFile->actions()[int(i)]->setCheckable(true);
        _spMenuFile->actions()[int(i)]->setChecked(true);
    }
}

Filter_Clog Viewer::createFilterFromPixFilterPanel()
{
    Filter_Clog filter;
    filter._clusterRangeBegin = _spPixFilterPanel->getClusterBegin();
    filter._clusterRangeEnd = _spPixFilterPanel->getClusterEnd();
    filter._totRangeBegin = _spPixFilterPanel->getTotBegin();
    filter._totRangeEnd = _spPixFilterPanel->getTotEnd();
    filter._isAllTotInCluster = _spPixFilterPanel->isAllTotInCluster();
    filter._isMidiPix = _spPixFilterPanel->isMediPix();
    return filter;
}

void Viewer::createButtonPanel()
{
    _spViewerButtonPanel.reset(new Viewer_Button_Panel);
    connect(_spViewerButtonPanel.get(), SIGNAL(signalRotatePlus()), this, SLOT(slotRotatePlus()));
    connect(_spViewerButtonPanel.get(), SIGNAL(signalRotateMinus()), this, SLOT(slotRotateMinus()));
    connect(_spViewerButtonPanel.get(), SIGNAL(signalMirrorHorizontal()), this, SLOT(slotMirrorHorizontal()));
    connect(_spViewerButtonPanel.get(), SIGNAL(signalMirrorVertical()), this, SLOT(slotMirrorVertical()));
    connect(_spViewerButtonPanel.get(), SIGNAL(signalResetTransform()), this, SLOT(slotResetTransform()));
    connect(_spViewerButtonPanel.get(), SIGNAL(signalScaledPlus()), this, SLOT(slotScaledPlus()));
    connect(_spViewerButtonPanel.get(), SIGNAL(signalScaledMinus()), this, SLOT(slotScaledMinus()));
    connect(_spViewerButtonPanel.get(), SIGNAL(signalSaveToBmp()), this, SLOT(slotSaveBMP()));
    connect(_spViewerButtonPanel.get(), SIGNAL(signalSaveToTxt()), this, SLOT(slotSaveTXT()));
    this->layout()->addWidget(_spViewerButtonPanel.get());
}

void Viewer::createDataPanel()
{
    if(_spViewerDataPanel)
        return;
    _spViewerDataPanel.reset(new Viewer_Data_Panel);
    if(_spViewerProcessor)
        _spViewerDataPanel->setData(0,0,0, _spViewerProcessor->getColumns(), _spViewerProcessor->getRows());
    if(_pEventFilterScene)
        connect(_pEventFilterScene, SIGNAL(signalMousePos(QPointF)), this, SLOT(slotViewPosition(QPointF)));
    this->layout()->addWidget(_spViewerDataPanel.get());
}

void Viewer::createPixFilterPanel()
{
    _spPixFilterPanel.reset(new Pix_Filter_Panel);
    ui->layout_graphicView_and_Pix_filter_panel->addWidget(_spPixFilterPanel.get());
    ui->layout_graphicView_and_Pix_filter_panel->setStretch(0,1);
    if(_spViewerProcessor && _spViewerProcessor->getFileType() != Viewer_Processor::fileType::CLOG)
        _spPixFilterPanel->setTabEnable(Pix_Filter_Panel::CLOG_FILTER_TAB, false);
    connectPixFilterPanel();
}

void Viewer::slotSetImageFile(QString file)
{
    setImageFile(file);
}
void Viewer::slotMoveRectFromKey()
{
    if(!_spPixFilterPanel)
        return;

    QGraphicsRectItem* rectItem = static_cast<QGraphicsRectItem*>(_pCurrentScene->items().at(0));

    rectItem->setPos(0, 0);
    rectItem->setRect(_spPixFilterPanel->getX(), _spPixFilterPanel->getY(),
                      _spPixFilterPanel->getWidth(), _spPixFilterPanel->getHeight());
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

void Viewer::slotPixAndFilterPanelMenuToggle()
{
    if( _state_of_the_menu_items[PIX_AND_FILTER_PANEL])
    {
        _spPixFilterPanel = nullptr;
        if(_itemRect)
        {
            _pCurrentScene->removeItem(_itemRect);
            _itemRect = nullptr;
        }
        ui->graphicsView->unsetCursor();
    }
    else
        createPixFilterPanel();

     _state_of_the_menu_items[PIX_AND_FILTER_PANEL] = ! _state_of_the_menu_items[PIX_AND_FILTER_PANEL];
}
void Viewer::slotDataPanelMenuToggle()
{
    if( _state_of_the_menu_items[DATA_PANEL])
        _spViewerDataPanel = nullptr;
    else
        createDataPanel();

     _state_of_the_menu_items[DATA_PANEL] = ! _state_of_the_menu_items[DATA_PANEL];
}
void Viewer::slotButtonPanelMenuToggle()
{
    if( _state_of_the_menu_items[BUTTONS_PANEL])
        _spViewerButtonPanel = nullptr;
    else
        createButtonPanel();

     _state_of_the_menu_items[BUTTONS_PANEL] = ! _state_of_the_menu_items[BUTTONS_PANEL];
}
void Viewer::slotInversionMenuToggle()
{
    ui->inversion->setVisible(!_state_of_the_menu_items[INVERSION]);
     _state_of_the_menu_items[INVERSION] = !_state_of_the_menu_items[INVERSION];
}
void Viewer::slotSeparateWindowMenuToggle()
{   
    Viewer* SW = new Viewer;
    SW->setWindowTitle(_filePath);
    SW->setSettings(_spSettings);
    SW->show();

    if(_spViewerProcessor  && _spViewerProcessor->getFileType() == Viewer_Processor::fileType::TXT)
        SW->setImageFile(_filePath);
//    else if(_pViewerProcessor  && _pViewerProcessor->getFileType() == Viewer_Processor::fileType::CLOG)
//        SW->setImageFile(_filePath);
//    else
//        SW->setImageFile(_filePath);
}

void Viewer::slotViewSelectionMovePos(QPoint point)
{
    disconnectPixFilterPanelSelectionBox();

    if(_spPixFilterPanel)
    {
        _spPixFilterPanel->setX(point.x());
        _spPixFilterPanel->setY(point.y());
    }

    connectPixFilterPanelSelectionBox();
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
        slotInversionCheckBox(ui->inversion->checkState());
        _spViewerProcessor->setDataInVec2D(static_cast<size_t>(x), static_cast<size_t>(y), _spPixFilterPanel->getPenValue());

    }
}
void Viewer::slotViewSelectionPos(QRect rect)
{
    int x = rect.x();
    int y = rect.y();

    int width = rect.width();
    int height = rect.height();

    if(height < 0 && width < 0)
    {
        x = x + width;
        y = y - qAbs(height);
        width = qAbs(width);
        height = qAbs(height);
    }
    else if(height < 0 && width > 0)
    {
        y = y - qAbs(height);
        width = qAbs(width);
        height = qAbs(height);
    }
    else if(height > 0 && width < 0  )
    {
        x = x + width;
        width = qAbs(width);
        height = qAbs(height);
    }
    else if(height > 0 && width > 0  )
    {
        ;
    }

    if(!_spPixFilterPanel)
        return;

    _spPixFilterPanel->setX(x);
    _spPixFilterPanel->setY(y);
    _spPixFilterPanel->setWidth(width);
    _spPixFilterPanel->setHeight(height);
}
void Viewer::slotFinishSelection()
{
    ui->graphicsView->unsetCursor();
    setReadOnlyDataPanelSelection(false);
    _spPixFilterPanel->finishSelection();
    _spPixFilterPanel->setButtonCutDisable(false);
}

void Viewer::incorrectFile()
{
    setEnableButtonPanel(false);
    setEmptyImageOnViewerScene();
    if(_spViewerDataPanel)
    {
        //Отображение на панели координаты курсора (x,y) относительно graphicsView
        disconnect(_pEventFilterScene, SIGNAL(signalMousePos(QPointF)), this, SLOT(slotViewPosition(QPointF)));
    }
    //Отображение на панели данных о выделении(x,y,width,height)
    disconnect(_pEventFilterScene, SIGNAL(siganlRect(QRect)), this, SLOT(slotViewSelectionPos(QRect)));
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
void Viewer::slotSelectionFrame(bool state)
{
    if(state)
    {
        setEnableDataPanelSelection(true);
        if(_itemRect)
        {
            _pCurrentScene->removeItem(_itemRect);
            _itemRect = nullptr;
        }
        ui->graphicsView->setCursor(QCursor(Qt::CrossCursor));
    }
    if(!state && _itemRect == nullptr)
    {
        ui->graphicsView->unsetCursor();
        setEnableDataPanelSelection(false);
    }
}

void Viewer::slotPen(bool value)
{
    if(value)
       ui->graphicsView->setCursor(QCursor(QPixmap(":/pen").scaled(24,24, Qt::KeepAspectRatio, Qt::SmoothTransformation), X_HOT ,Y_HOT));
    else
       ui->graphicsView->unsetCursor();
}
void Viewer::slotInversionCheckBox(int state)
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
    {
        std::cerr << __FUNCTION__ << " pPixFilterPanel=nullptr" << std::endl;
        exit(1);
    }
    std::vector<std::vector<double>> newVec2D = _spViewerProcessor->cutVec2D(static_cast<size_t>(_spPixFilterPanel->getX()),
                                                                             static_cast<size_t>(_spPixFilterPanel->getY()),
                                                                             static_cast<size_t>(_spPixFilterPanel->getWidth()),
                                                                             static_cast<size_t>(_spPixFilterPanel->getHeight()));
    QString fileName = Saver::saveInTemporaryTXT(static_cast<size_t>(_spPixFilterPanel->getWidth()),
                                           static_cast<size_t>(_spPixFilterPanel->getHeight()),
                                           newVec2D);
    setImageFile(fileName);
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
    if(!_spViewerDataPanel)
        return;

    int width = static_cast<int>(_pCurrentScene->sceneRect().width());
    int height = static_cast<int>(_pCurrentScene->sceneRect().height());

    if(pos.x() < 0 || pos.x() > width || pos.y() < 0 || pos.y() > height)
    {
        _spViewerDataPanel->setX(0);
        _spViewerDataPanel->setY(0);
        _spViewerDataPanel->setData(0.0);
        return;
    }
    _spViewerDataPanel->setX(static_cast<int>(pos.x()));
    _spViewerDataPanel->setY(static_cast<int>(pos.y()));
    _spViewerDataPanel->setData(_spViewerProcessor->getDataInVec2D(static_cast<size_t>(pos.x()), static_cast<size_t>(pos.y())));
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
    Saver::dialogSaveTXT(_spViewerProcessor->getColumns(), _spViewerProcessor->getRows(), _spViewerProcessor->getVec2D());
}
Viewer::~Viewer()
{
    delete ui;
    // !!!!!!!!!!!!!!!!!!!!
    delete _pEventFilterScene;
//    delete eventFilterViewPort;

}
