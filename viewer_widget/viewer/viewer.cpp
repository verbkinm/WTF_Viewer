#include <QPushButton>
#include <QTransform>
#include <QFileDialog>
#include <QTabBar>
#include <QPixmap>
#include <QMessageBox>
#include <QFile>
#include <QFileInfo>
#include <QtMath>

#include <QDebug>
#include <QTime>

#include "viewer.h"
#include "ui_viewer.h"
#include "../checkFile/checkfile.h"
#include "../../progressbar.h"
#include "saver.h"

Viewer::Viewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Viewer), _pViewerProcessor(nullptr), _pViewerButtonPanel(nullptr), _pViewerDataPanel(nullptr), _pPixFilterPanel(nullptr),
    _pSettings(nullptr),
    _readOnly(false),
    _itemForeground(nullptr), _itemRect(nullptr),
    _eventFilterScene(nullptr),
    _pMenuFile(nullptr),
    _pCurrentScene(nullptr)
{ 
    this->setAttribute(Qt::WA_DeleteOnClose);

    _pCurrentScene = &_defaultScene;

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
    _eventFilterScene.reset(new FingerSlide(_pCurrentScene));
    _pCurrentScene->installEventFilter(_eventFilterScene.get());

    //представление
    ui->graphicsView->viewport()->setObjectName("viewport");

    //фильтр событий для представления
    FingerSlide* eventFilterViewPort = new FingerSlide(ui->graphicsView->viewport());
    ui->graphicsView->viewport()->installEventFilter(eventFilterViewPort);
    connect(eventFilterViewPort, SIGNAL(signalWheel(int)), this, SLOT(slotScaleWheel(int)));

    //при первом запуске - вывести на экран надпись и отключить всё не нужное
    incorrectFile();
}
void Viewer::selectFile()
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
    for(size_t i = 0; i < sizeof( _state_of_the_menu_items) / sizeof ( _state_of_the_menu_items[0]) - 1; ++i)
        _pMenuFile->actions()[int(i)]->trigger();
}
void Viewer::hideSettingsButton(bool state)
{
    _pMenuFile = nullptr;
    ui->button_settings->setVisible(!state);
    ui->marker_label->setVisible(!state);
}

void Viewer::setEnableDataPanelSelection(bool state)
{
    disconnectPixFilterPanelSelectionBox();

    if(_pPixFilterPanel)
    {
        _pPixFilterPanel->setDataPanelEnabled(state);
        _pPixFilterPanel->setX(0);
        _pPixFilterPanel->setY(0);
        _pPixFilterPanel->setWidth(0);
        _pPixFilterPanel->setHeight(0);
    }

    setReadOnlyDataPanelSelection(true);

    connectPixFilterPanelSelectionBox();
}
void Viewer::setReadOnlyDataPanelSelection(bool state)
{
    if(_pPixFilterPanel)
        _pPixFilterPanel->setDataPanelReadOnly(state);
}
void Viewer::setEnableButtonPanel(bool state)
{
    if(_pViewerButtonPanel )
        _pViewerButtonPanel->setEnabled(state);
    if(_pViewerDataPanel)
        _pViewerDataPanel->setEnabled(state);

    ui->button_settings->setEnabled(state);
    ui->inversion->setEnabled(state);

    if(_pPixFilterPanel)
        _pPixFilterPanel->setEnabled(state);
    if(_pViewerProcessor  && _pViewerProcessor->getFileType() == Viewer_Processor::fileType::TXT && _pPixFilterPanel)
        _pPixFilterPanel->setTabEnable(Pix_Filter_Panel::CLOG_FILTER_TAB, false);
    else if(_pViewerProcessor  && _pViewerProcessor->getFileType() == Viewer_Processor::fileType::CLOG && _pPixFilterPanel)
    {
        _pPixFilterPanel->setTabEnable(Pix_Filter_Panel::CLOG_FILTER_TAB, true);
        _pPixFilterPanel->setClusterRange(std::static_pointer_cast<Viewer_Clog_Processor>(_pViewerProcessor).get()->getClustersLengthVector());
        _pPixFilterPanel->setTotRange(std::static_pointer_cast<Viewer_Clog_Processor>(_pViewerProcessor).get()->getVectorOfLengthsOfTots());
    }
    setEnableDataPanelSelection(false);
}
void Viewer::setImage(QImage image)
{
    _currentImage = image;
    if(image.format() != QImage::Format_Invalid)
    {
        setEnableButtonPanel(true);
        showMarkers();
        ui->graphicsView->resetTransform();
        _pCurrentScene->setSceneRect(image.rect());
        if(_pViewerDataPanel)
        {
            _pViewerDataPanel->setData(0,0, 0.0, static_cast<size_t>(_pCurrentScene->width()), static_cast<size_t>(_pCurrentScene->height()) );
            //Отображение на панели координаты курсора (x,y) относительно graphicsView
            connect(_eventFilterScene.get(), SIGNAL(signalMousePos(QPointF)), this, SLOT(slotViewPosition(QPointF)));
        }
        ui->graphicsView->fitInView(image.rect(), Qt::KeepAspectRatio);
        _pCurrentScene->clear();
        _itemRect = nullptr;
        _itemForeground = _pCurrentScene->addPixmap(QPixmap::fromImage(_currentImage));
        _itemForeground->setZValue(1);

        slotInversionCheckBox(ui->inversion->checkState());
        connectEventFilter();
    }
    else
        incorrectFile();
}
void Viewer::setSettings(std::shared_ptr<QSettings> pShareSettings)
{
    _pSettings = pShareSettings;
}
void Viewer::setScene(QGraphicsScene *scene)
{
    ui->graphicsView->setScene(scene);
    _pCurrentScene = scene;
}
void Viewer::setSceneDefault()
{
    _pCurrentScene = &_defaultScene;
    ui->graphicsView->setScene(_pCurrentScene);
}
void Viewer::setImageFile(QString &fileName)
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    _filePath = fileName;
    QFileInfo file(_filePath);

    if(file.suffix() == "txt")
    {
        _pViewerProcessor = std::make_shared<Viewer_Txt_Processor>();

        _pMenuFile->actions()[PIX_AND_FILTER_PANEL]->setDisabled(false);
        _pViewerProcessor->setSettings(_pSettings);
        _pViewerProcessor->setFileName(fileName);
        setImage(_pViewerProcessor->getImage());
    }
    else if(file.suffix() == "clog")
    {
        if(!_pPixFilterPanel)
        {
            QApplication::restoreOverrideCursor();
            QMessageBox::critical(this, "Error", "Please, enable \"pix. & filter panel\"!");
            return;
        }
        _pViewerProcessor = std::make_shared<Viewer_Clog_Processor>();

        _pMenuFile->actions()[PIX_AND_FILTER_PANEL]->setDisabled(true);
        _pViewerProcessor->setSettings(_pSettings);
        _pViewerProcessor->setFileName(fileName);
        setEnableButtonPanel(true);

        std::static_pointer_cast<Viewer_Clog_Processor>(_pViewerProcessor).get()->setFilter(createFilterFromPixFilterPanel());
        setImage(_pViewerProcessor->getImage());
    }
    else
        incorrectFile();

    QApplication::restoreOverrideCursor();
}
void Viewer::disconnectPixFilterPanelSelectionBox()
{
    if(!_pPixFilterPanel)
        return;
    disconnect(_pPixFilterPanel.get(), SIGNAL(signalValueX_Changed(int)), this, SLOT(slotMoveRectFromKey()) );
    disconnect(_pPixFilterPanel.get(), SIGNAL(signalValueY_Changed(int)), this, SLOT(slotMoveRectFromKey()) );
    disconnect(_pPixFilterPanel.get(), SIGNAL(signalValueWidth_Changed(int)), this, SLOT(slotMoveRectFromKey()) );
    disconnect(_pPixFilterPanel.get(), SIGNAL(signalValueHeight_Changed(int)), this, SLOT(slotMoveRectFromKey()) );
}

void Viewer::connectPixFilterPanel()
{
    //соединение сигналов высылаемых классом ToolsPanel
    connect(_pPixFilterPanel.get(), SIGNAL(signalSelectionToggle(bool)), this, SLOT(slotSelectionFrame(bool)));
    connect(_pPixFilterPanel.get(), SIGNAL(signalPenToggle(bool)), this, SLOT(slotPen(bool)));
    connect(_pPixFilterPanel.get(), SIGNAL(signalCutClicked(bool)), this, SLOT(slotCut()));
    connect(_pPixFilterPanel.get(), SIGNAL(signalRepaint()), this, SLOT(slotRepaint()));

    connectPixFilterPanelSelectionBox();

//    //Нажатия на кнопку Apply на панели фильтров clog
    connect(_pPixFilterPanel.get(), SIGNAL(signalApplyFilter()),       this, SLOT(slotApplyClogFilter()));
}

void Viewer::connectPixFilterPanelSelectionBox()
{
    if(!_pPixFilterPanel)
        return;
    //изменение выделения с помощью спинбоксов
    connect(_pPixFilterPanel.get(), SIGNAL(signalValueX_Changed(int)), this, SLOT(slotMoveRectFromKey()) );
    connect(_pPixFilterPanel.get(), SIGNAL(signalValueY_Changed(int)), this, SLOT(slotMoveRectFromKey()) );
    connect(_pPixFilterPanel.get(), SIGNAL(signalValueWidth_Changed(int)), this, SLOT(slotMoveRectFromKey()) );
    connect(_pPixFilterPanel.get(), SIGNAL(signalValueHeight_Changed(int)), this, SLOT(slotMoveRectFromKey()) );
}

void Viewer::connectEventFilter()
{
    //Отображение на панели данных о выделении(x,y,width,height)
    connect(_eventFilterScene.get(), SIGNAL(siganlRect(QRect)), this, SLOT(slotViewSelectionPos(QRect)));
    //Отображение на панели координат выделения при перемещении(x,y)
    connect(_eventFilterScene.get(), SIGNAL(signalRectMove(QPoint)), this, SLOT(slotViewSelectionMovePos(QPoint)));
    //Изображение курсора - стрелка, выключение кнопки selection_button
    connect(_eventFilterScene.get(), SIGNAL(signalRelease()), this, SLOT(slotFinishSelection()));

    connect(_eventFilterScene.get(), SIGNAL(signalCreateRectItem(QGraphicsRectItem*)), this, SLOT(slotCreateRectItem(QGraphicsRectItem*)));
    connect(_eventFilterScene.get(), SIGNAL(signalDrawPoint(QPointF)), this, SLOT(slotDrawPoint(QPointF)));
}

void Viewer::showMarkers()
{
    QString border = "Border; ";
    QString mask = "Mask; ";
    QString generalCalibration = "General calibration; ";

    switch (_pViewerProcessor->_markers.operator unsigned int())
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
    _pMenuFile.reset(new QMenu);

    _pMenuFile->addAction("pix. && filter panel", this,
                         SLOT(slotPixAndFilterPanelMenuToggle()));
    _pMenuFile->addAction("data pnael", this,
                         SLOT(slotDataPanelMenuToggle()));
    _pMenuFile->addAction("buttons panel", this,
                         SLOT(slotButtonPanelMenuToggle()));
    _pMenuFile->addAction("inversion", this,
                         SLOT(slotInversionMenuToggle()));
    _pMenuFile->addSeparator();
    _pMenuFile->addAction("separate window", this,
                         SLOT(slotSeparateWindowMenuToggle()));

    ui->button_settings->setMenu(_pMenuFile.get());

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
    unsigned int lenght = sizeof( _state_of_the_menu_items) / sizeof ( _state_of_the_menu_items[0]) - 1; // -1 - для последнего пункта не надо
    for (unsigned int i = 0; i < lenght; ++i)
    {
        _pMenuFile->actions()[int(i)]->setCheckable(true);
        _pMenuFile->actions()[int(i)]->setChecked(true);
    }
}

Filter_Clog Viewer::createFilterFromPixFilterPanel()
{
    Filter_Clog filter;
    filter._clusterRangeBegin = _pPixFilterPanel->getClusterBegin();
    filter._clusterRangeEnd = _pPixFilterPanel->getClusterEnd();
    filter._totRangeBegin = _pPixFilterPanel->getTotBegin();
    filter._totRangeEnd = _pPixFilterPanel->getTotEnd();
    filter._isAllTotInCluster = _pPixFilterPanel->isAllTotInCluster();
    filter._isMidiPix = _pPixFilterPanel->isMediPix();
    return filter;
}

void Viewer::createButtonPanel()
{
    _pViewerButtonPanel.reset(new Viewer_Button_Panel);
    //повороты
    connect(_pViewerButtonPanel.get(), SIGNAL(signalRotatePlus()), this, SLOT(slotRotatePlus()));
    connect(_pViewerButtonPanel.get(), SIGNAL(signalRotateMinus()), this, SLOT(slotRotateMinus()));
    //зеркальное отражение
    connect(_pViewerButtonPanel.get(), SIGNAL(signalMirrorHorizontal()), this, SLOT(slotMirrorHorizontal()));
    connect(_pViewerButtonPanel.get(), SIGNAL(signalMirrorVertical()), this, SLOT(slotMirrorVertical()));
    //сброс трансформации
    connect(_pViewerButtonPanel.get(), SIGNAL(signalResetTransform()), this, SLOT(slotResetTransform()));
    //масштаб
    connect(_pViewerButtonPanel.get(), SIGNAL(signalScaledPlus()), this, SLOT(slotScaledPlus()));
    connect(_pViewerButtonPanel.get(), SIGNAL(signalScaledMinus()), this, SLOT(slotScaledMinus()));
    //сохранение в bmp и txt
    connect(_pViewerButtonPanel.get(), SIGNAL(signalSaveToBmp()), this, SLOT(slotSaveBMP()));
    connect(_pViewerButtonPanel.get(), SIGNAL(signalSaveToTxt()), this, SLOT(slotSaveTXT()));

    this->layout()->addWidget(_pViewerButtonPanel.get());
}

void Viewer::createDataPanel()
{
    if(_pViewerDataPanel)
        return;
    _pViewerDataPanel.reset(new Viewer_Data_Panel);
    if(_pViewerProcessor )
        _pViewerDataPanel->setData(0,0,0, _pViewerProcessor->getColumns(), _pViewerProcessor->getRows());
    if(_eventFilterScene)
        connect(_eventFilterScene.get(), SIGNAL(signalMousePos(QPointF)), this, SLOT(slotViewPosition(QPointF)));
    this->layout()->addWidget(_pViewerDataPanel.get());
}

void Viewer::createPixFilterPanel()
{
    _pPixFilterPanel.reset(new Pix_Filter_Panel);

    ui->layout_graphicView_and_Pix_filter_panel->addWidget(_pPixFilterPanel.get());
    ui->layout_graphicView_and_Pix_filter_panel->setStretch(0,1);

    if(_pViewerProcessor  && _pViewerProcessor->getFileType() != Viewer_Processor::fileType::CLOG)
        _pPixFilterPanel->setTabEnable(Pix_Filter_Panel::CLOG_FILTER_TAB, false);
    connectPixFilterPanel();
}

void Viewer::slotSetImageFile(QString file)
{
    setImageFile(file);
}
void Viewer::slotMoveRectFromKey()
{
    if(!_pPixFilterPanel)
        return;

    QGraphicsRectItem* rectItem = static_cast<QGraphicsRectItem*>(_pCurrentScene->items().at(0));

    rectItem->setPos(0, 0);
    rectItem->setRect(_pPixFilterPanel->getX(), _pPixFilterPanel->getY(),
                      _pPixFilterPanel->getWidth(), _pPixFilterPanel->getHeight());
}
void Viewer::slotCreateRectItem(QGraphicsRectItem * item)
{
    _itemRect = item;
}
void Viewer::slotApplyClogFilter()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    Filter_Clog filter = createFilterFromPixFilterPanel();
    std::static_pointer_cast<Viewer_Clog_Processor>(_pViewerProcessor).get()->setFilter(filter);
    setImage(_pViewerProcessor->getImage());
    QApplication::restoreOverrideCursor();
}
void Viewer::slotRepaint()
{
    setImage(_pViewerProcessor->getRedrawnImage());
}

void Viewer::slotPixAndFilterPanelMenuToggle()
{
    if( _state_of_the_menu_items[PIX_AND_FILTER_PANEL])
    {
        _pPixFilterPanel = nullptr;
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
        _pViewerDataPanel = nullptr;
    else
        createDataPanel();

     _state_of_the_menu_items[DATA_PANEL] = ! _state_of_the_menu_items[DATA_PANEL];
}
void Viewer::slotButtonPanelMenuToggle()
{
    if( _state_of_the_menu_items[BUTTONS_PANEL])
        _pViewerButtonPanel = nullptr;
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
    SW->setSettings(_pSettings);
    SW->show();

    if(_pViewerProcessor  && _pViewerProcessor->getFileType() == Viewer_Processor::fileType::TXT)
        SW->setImageFile(_filePath);
    else if(_pViewerProcessor  && _pViewerProcessor->getFileType() == Viewer_Processor::fileType::CLOG)
        SW->setImageFile(_filePath);
    else
        SW->setImageFile(_filePath);
}

void Viewer::slotViewSelectionMovePos(QPoint point)
{
    disconnectPixFilterPanelSelectionBox();

    if(_pPixFilterPanel)
    {
        _pPixFilterPanel->setX(point.x());
        _pPixFilterPanel->setY(point.y());
    }

    connectPixFilterPanelSelectionBox();
}

void Viewer::slotDrawPoint(QPointF point)
{
    if(!_pPixFilterPanel)
        return;

    int x = static_cast<int>(point.x());
    int y = static_cast<int>(point.y());

    if(x >= 0 && x < static_cast<int>(_pViewerProcessor->getColumns()) && y >= 0 && y < static_cast<int>(_pViewerProcessor->getRows()))
    {
        _currentImage.setPixelColor(x, y, _pPixFilterPanel->getPenColor());
        slotInversionCheckBox(ui->inversion->checkState());
        _pViewerProcessor->setDataInVec2D(static_cast<size_t>(x), static_cast<size_t>(y), _pPixFilterPanel->getPenValue());

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

    if(!_pPixFilterPanel)
        return;

    _pPixFilterPanel->setX(x);
    _pPixFilterPanel->setY(y);
    _pPixFilterPanel->setWidth(width);
    _pPixFilterPanel->setHeight(height);
}
void Viewer::slotFinishSelection()
{
    ui->graphicsView->unsetCursor();
    setReadOnlyDataPanelSelection(false);
    _pPixFilterPanel->finishSelection();
    _pPixFilterPanel->setButtonCutDisable(false);
}

void Viewer::incorrectFile()
{
    setEnableButtonPanel(false);
    selectFile();
    if(_pViewerDataPanel)
    {
        //Отображение на панели координаты курсора (x,y) относительно graphicsView
        disconnect(_eventFilterScene.get(), SIGNAL(signalMousePos(QPointF)), this, SLOT(slotViewPosition(QPointF)));
    }
    //Отображение на панели данных о выделении(x,y,width,height)
    disconnect(_eventFilterScene.get(), SIGNAL(siganlRect(QRect)), this, SLOT(slotViewSelectionPos(QRect)));
    //Отображение на панели координат выделения при перемещении(x,y)
    disconnect(_eventFilterScene.get(), SIGNAL(signalRectMove(QPoint)), this, SLOT(slotViewSelectionMovePos(QPoint)));
    //Изображение курсора - стрелка, выключение кнопки edit
    disconnect(_eventFilterScene.get(), SIGNAL(signalRelease()), this, SLOT(slotFinishSelection()));

    disconnect(_eventFilterScene.get(), SIGNAL(signalCreateRectItem(QGraphicsRectItem*)), this, SLOT(slotCreateRectItem(QGraphicsRectItem*)));
    disconnect(_eventFilterScene.get(), SIGNAL(signalDrawPoint(QPointF)), this, SLOT(slotDrawPoint(QPointF)));

    if(_pViewerDataPanel)
        _pViewerDataPanel->setData(0,0,0.0,0,0);
}

void Viewer::resetTransform()
{
    ui->graphicsView->resetTransform();
}

QWidget *Viewer::getViewport() const
{
    return ui->graphicsView->viewport();
}

QGraphicsScene *Viewer::getScene()
{
    if(_pCurrentScene)
        return _pCurrentScene;
    else
        return &_defaultScene;
}
void Viewer::slotScaleWheel(int value)
{
    if(value > 0)
    {
        ui->graphicsView->scale(1.1, 1.1);
    }
    else if(value < 0)
    {
        ui->graphicsView->scale(1 / 1.1, 1 / 1.1);
    }
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
        _itemForeground->setPixmap(QPixmap::fromImage(_currentImage));
        break;
    case (Qt::Checked):
        QImage inversion(_currentImage);
        inversion.invertPixels(QImage::InvertRgb);
        _itemForeground->setPixmap(QPixmap::fromImage(inversion));
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
    if(!_pPixFilterPanel)
    {
        qDebug() << __FUNCTION__ << " pPixFilterPanel=nullptr";
        exit(1);
    }
    std::vector<std::vector<double>> newVec2D = _pViewerProcessor->cutVec2D(static_cast<size_t>(_pPixFilterPanel->getX()),
                                                                            static_cast<size_t>(_pPixFilterPanel->getY()),
                                                                            static_cast<size_t>(_pPixFilterPanel->getWidth()),
                                                                            static_cast<size_t>(_pPixFilterPanel->getHeight()));
    QString fileName = Saver::temporaryTXT(static_cast<size_t>(_pPixFilterPanel->getWidth()),
                                           static_cast<size_t>(_pPixFilterPanel->getHeight()),
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
    if(!_pViewerDataPanel)
        return;

    int width = static_cast<int>(_pCurrentScene->sceneRect().width());
    int height = static_cast<int>(_pCurrentScene->sceneRect().height());

    if(pos.x() < 0 || pos.x() > width || pos.y() < 0 || pos.y() > height)
    {
        _pViewerDataPanel->setX(0);
        _pViewerDataPanel->setY(0);
        _pViewerDataPanel->setData(0.0);
        return;
    }

    _pViewerDataPanel->setX(static_cast<int>(pos.x()));
    _pViewerDataPanel->setY(static_cast<int>(pos.y()));
    _pViewerDataPanel->setData(_pViewerProcessor->getDataInVec2D(static_cast<size_t>(pos.x()), static_cast<size_t>(pos.y())));
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

    Saver::saveBMP(image);
}

void Viewer::slotSaveTXT()
{
    Saver::saveTXT(_pViewerProcessor->getColumns(), _pViewerProcessor->getRows(), _pViewerProcessor->getVec2D());
}
Viewer::~Viewer()
{
    delete ui;
}

template<typename T>
void Viewer::objectDelete(T* obj)
{
    if(obj != nullptr)
        delete obj;
}
