#include <QPushButton>
#include <QTransform>
#include <QFileDialog>
#include <QTabBar>
#include <QPixmap>
#include <QMessageBox>
#include <QFile>
#include <QFileInfo>
#include <QTemporaryFile>
#include <QtMath>

#include <QDebug>
#include <QTime>

#include "viewer.h"
#include "ui_viewer.h"
#include "checkFile/checkfile.h"
#include "../progressbar.h"

Viewer::Viewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Viewer)
{ 

    this->setAttribute(Qt::WA_DeleteOnClose);

    currentScene = &defaultScene;
    for (unsigned int i = 0; i < sizeof(action_array) / sizeof (action_array[0]) - 1; ++i)
        action_array[i] = true;

    ui->setupUi(this);

    createButtonMenu();
    createDataPanel();
    createButtonPanel();
    createPixFilterPanel();

    //инверсия цвета
    connect(ui->inversion, SIGNAL(stateChanged(int)), this, SLOT(slotInversionCheckBox(int)));

    //установка сцены
    currentScene->setObjectName("scene");
    ui->graphicsView->setScene(currentScene);
    //фильтр событий для сцены
    eventFilterScene = new FingerSlide(currentScene);
    currentScene->installEventFilter(eventFilterScene);

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
    currentScene->clear();

    ui->graphicsView->resetTransform();

    currentScene->setSceneRect(QRect(0,0, 50,50));
    currentScene->addText("Select file!");
}
size_t Viewer::getColumnFromFile(QString fileName) const
{
    return ListData(fileName).column;
}
size_t Viewer::getRowFromFile(QString fileName) const
{
    return ListData(fileName).row;
}
void Viewer::setReadOnly(bool value)
{
    readOnly = value;

    if(itemRect)
    {
        itemRect->setFlag(QGraphicsItem::ItemIsMovable, !value);
        itemRect->setFlag(QGraphicsItem::ItemIsSelectable, !value);
    }
    if(value)
        currentScene->setObjectName("NONE");
    else
        currentScene->setObjectName("scene");
}
void Viewer::hideAllPanel()
{
    for(size_t i = 0; i < sizeof(action_array) / sizeof (action_array[0]) - 1; ++i)
        pMenuFile->actions()[int(i)]->trigger();
}
void Viewer::hideSettingsButton(bool value)
{
    objectDelete(pMenuFile);
    pMenuFile = nullptr;
    ui->button_settings->setVisible(!value);
    ui->marker_label->setVisible(!value);
}
Frames *Viewer::getFrames()
{
    return &frames;
}
QImage Viewer::getImageFromTxtFile(QString fileName)
{
    return createArrayImage(fileName);
}
QImage Viewer::getImageFromClogFile(QString fileName)
{
    ProgressBar progressBar(0);
    connect(&frames, SIGNAL(signalFramesCreated()), &progressBar, SLOT(close()));
    progressBar.show();

    frames.setFile(fileName);
    setEnableButtonPanel(true);

    column  = CLOG_SIZE;
    row     = CLOG_SIZE;

    QImage image(int(column), int(row), QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::black);

    //выделяем память для основного массива
    arrayOrigin = new double *[column];
    for (size_t  i = 0; i < column; ++i)
        arrayOrigin[i] = new double[row];

    //наполняем основной массив данными согласно установленному фильтру
    applyClogFilter(image);

    return image;
}
void Viewer::clearArrayOrigin()
{
    if(arrayOrigin != nullptr){
        for (size_t  i = 0; i < column; ++i)
            delete[] arrayOrigin[i];
        delete[] arrayOrigin;
        arrayOrigin = nullptr;
        column = 0;
        row = 0;

        itemForeground = nullptr;
        itemRect       = nullptr;
    }
}
void Viewer::setEnableDataPanelSelection(bool state)
{
    disconnectPixFilterPanelSelectionBox();

    if(pPixFilterPanel)
    {
        pPixFilterPanel->setDataPanelEnabled(state);
        pPixFilterPanel->setX(0);
        pPixFilterPanel->setY(0);
        pPixFilterPanel->setWidth(0);
        pPixFilterPanel->setHeight(0);
    }

    setReadOnlyDataPanelSelection(true);

    connectPixFilterPanelSelectionBox();
}
void Viewer::setReadOnlyDataPanelSelection(bool state)
{
    if(pPixFilterPanel)
        pPixFilterPanel->setDataPanelReadOnly(state);
}
void Viewer::setEnableButtonPanel(bool state)
{
    if(pViewerButtonPanel)
        pViewerButtonPanel->setEnabled(state);
    if(pViewerDataPanel)
        pViewerDataPanel->setEnabled(state);

    ui->inversion->setEnabled(state);

    if(pPixFilterPanel)
    {
        pPixFilterPanel->setEnabled(state);
        pPixFilterPanel->setToggleButtonEnable(false);
    }
    if(fType == fileType::TXT && pPixFilterPanel)
    {
        pPixFilterPanel->setTabEnable(Pix_Filter_Panel::CLOG_FILTER_TAB, false);
    }
    else if(fType == fileType::CLOG && pPixFilterPanel)
    {
        pPixFilterPanel->setTabEnable(Pix_Filter_Panel::CLOG_FILTER_TAB, true);
        pPixFilterPanel->setClusterRange(frames.getClustersLenghtList());
        pPixFilterPanel->setTotRange(frames.getTotLenghtList());
    }

    //state
    setEnableDataPanelSelection(false);
}
void Viewer::setImage(QImage image)
{
    imageOrigin = image;

    if(image.format() != QImage::Format_Invalid)
    {
        setEnableButtonPanel(true);

//        angle = 0;
//        ui->angle->setValue(0);
        ui->graphicsView->resetTransform();
        currentScene->setSceneRect(image.rect());

        if(pViewerDataPanel)
        {
            pViewerDataPanel->setAllData(0,0, 0.0, static_cast<int>(currentScene->width()), static_cast<int>(currentScene->height()) );
            //Отображение на панели координаты курсора (x,y) относительно graphicsView
            connect(eventFilterScene, SIGNAL(signalMousePos(QPointF)), this, SLOT(slotViewPosition(QPointF)));
        }
        ui->graphicsView->fitInView(image.rect(), Qt::KeepAspectRatio);

        currentScene->clear();

        itemForeground = currentScene->addPixmap(QPixmap::fromImage(imageOrigin));
        itemForeground->setZValue(1);

        slotInversionCheckBox(ui->inversion->checkState());

        //Отображение на панели данных о выделении(x,y,width,height)
        connect(eventFilterScene, SIGNAL(siganlRect(QRect)), this, SLOT(slotViewSelectionPos(QRect)));
        //Отображение на панели координат выделения при перемещении(x,y)
        connect(eventFilterScene, SIGNAL(signalRectMove(QPoint)), this, SLOT(slotViewSelectionMovePos(QPoint)));
        //Изображение курсора - стрелка, выключение кнопки selection_button
        connect(eventFilterScene, SIGNAL(signalRelease()), this, SLOT(slotFinishSelection()));

        connect(eventFilterScene, SIGNAL(signalCreateRectItem(QGraphicsRectItem*)), this, SLOT(slotCreateRectItem(QGraphicsRectItem*)));
        connect(eventFilterScene, SIGNAL(signalDrawPoint(QPointF)), this, SLOT(slotDrawPoint(QPointF)));
    }
    else
        incorrectFile();
}
void Viewer::setSettings(QSettings &settings)
{
    pSettings = &settings;
}
void Viewer::setScene(QGraphicsScene *scene)
{
    ui->graphicsView->setScene(scene);
    currentScene = scene;
}
void Viewer::setSceneDefault()
{
    currentScene = &defaultScene;
    ui->graphicsView->setScene(currentScene);
}
void Viewer::setImageFile(QString fileName)
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    filePath = fileName;
    QFileInfo file(filePath);

    frames.clear();
    clearArrayOrigin();

    if(file.suffix() == "txt")
    {
        fType = fileType::TXT;
        setImage(getImageFromTxtFile(filePath));
    }
    else if(file.suffix() == "clog")
    {
        if(!pPixFilterPanel)
        {
            QApplication::restoreOverrideCursor();
            QMessageBox::critical(this, "Error", "Please, enable \"pix. & filter panel\"!");
            return;
        }
        fType = fileType::CLOG;
        setImage(getImageFromClogFile(filePath));
    }
    else {
        fType = fileType::UNDEFINED;
        incorrectFile();
    }

    QApplication::restoreOverrideCursor();
}
void Viewer::disconnectPixFilterPanelSelectionBox()
{
    if(!pPixFilterPanel)
        return;
    disconnect(pPixFilterPanel, SIGNAL(signalValueX_Changed(int)), this, SLOT(slotMoveRectFromKey()) );
    disconnect(pPixFilterPanel, SIGNAL(signalValueY_Changed(int)), this, SLOT(slotMoveRectFromKey()) );
    disconnect(pPixFilterPanel, SIGNAL(signalValueWidth_Changed(int)), this, SLOT(slotMoveRectFromKey()) );
    disconnect(pPixFilterPanel, SIGNAL(signalValueHeight_Changed(int)), this, SLOT(slotMoveRectFromKey()) );
}

void Viewer::connectPixFilterPanel()
{
    //соединение сигналов высылаемых классом ToolsPanel
    connect(pPixFilterPanel, SIGNAL(signalSelectionToggle(bool)), this, SLOT(slotSelectionFrame(bool)));
    connect(pPixFilterPanel, SIGNAL(signalPenToggle(bool)), this, SLOT(slotPen(bool)));
    connect(pPixFilterPanel, SIGNAL(signalCutClicked(bool)), this, SLOT(slotCut()));
    connect(pPixFilterPanel, SIGNAL(signalRepaint()), this, SLOT(slotRepaint()));

    connectPixFilterPanelSelectionBox();

//    //Нажатия на кнопку Apply на панели фильтров clog
    connect(pPixFilterPanel, SIGNAL(signalApplyFilter()),       this, SLOT(slotApplyClogFilter()));
}

void Viewer::connectPixFilterPanelSelectionBox()
{
    if(!pPixFilterPanel)
        return;
    //изменение выделения с помощью спинбоксов
    connect(pPixFilterPanel, SIGNAL(signalValueX_Changed(int)), this, SLOT(slotMoveRectFromKey()) );
    connect(pPixFilterPanel, SIGNAL(signalValueY_Changed(int)), this, SLOT(slotMoveRectFromKey()) );
    connect(pPixFilterPanel, SIGNAL(signalValueWidth_Changed(int)), this, SLOT(slotMoveRectFromKey()) );
    connect(pPixFilterPanel, SIGNAL(signalValueHeight_Changed(int)), this, SLOT(slotMoveRectFromKey()) );
}

void Viewer::applyClogFilter(QImage& image)
{
    if(!pPixFilterPanel)
    {
        pMenuFile->actions()[PIX_AND_FILTER_PANEL]->trigger();
    }

    //обнуляем основной массив
   for (size_t  x = 0; x < column; ++x)
       for (size_t  y = 0; y < row; ++y)
           arrayOrigin[x][y] = 0;

    for (int frameNumber = 0; frameNumber < frames.getFrameCount(); ++frameNumber)
        for (int clusterNumber = 0; clusterNumber < frames.getClusterCount(frameNumber); ++clusterNumber)
        {
            if( frames.isClusterInRange(frames.getClusterLenght(frameNumber, clusterNumber),
                                      pPixFilterPanel->getClusterBegin(),
                                      pPixFilterPanel->getClusterEnd())
                                      &&
                frames.isTotInRange(frameNumber, clusterNumber,
                                  pPixFilterPanel->getTotBegin(),
                                  pPixFilterPanel->getTotEnd()) )
            {
                if(pPixFilterPanel->isAllTotInCluster())
                    for (int eventNumber = 0; eventNumber < frames.getEventCountInCluster(frameNumber, clusterNumber); ++eventNumber)
                    {
                        ePoint point = frames.getEPoint(frameNumber, clusterNumber, eventNumber);
                        applyClogFilterAdditionalFunction(point);
                    }
                else
                {
                    QList<ePoint> listePoint = frames.getListTotInRange(frameNumber, clusterNumber,
                                                                        pPixFilterPanel->getTotBegin(), pPixFilterPanel->getTotEnd());
                    for (auto &point : listePoint)
                        applyClogFilterAdditionalFunction(point);
                }
            }
        }

    //проверяем настройки для картинки такие как рамка и маскирование пикселей
    imageSettingsForArray();



    double max = findMaxInArrayOrigin();
    double min = findMinInArrayOrigin();

    //наполнение объекта QImage
    for (size_t  x = 0; x < column; ++x)
        for (size_t  y = 0; y < row; ++y) {
            double value = convert(arrayOrigin[x][y], \
                                            min, \
                                            max, \
                                            double(0), \
                                            double(255) );
            QColor color(qRound(value), qRound(value), qRound(value));
            image.setPixelColor(int(x), int(y), color);
        }
    //цвет пикселей, к которым применилась маска
    imageSettingsForImage(image);

}
//для меньшего кол-ва строк исполбзуем эту функцию
void Viewer::applyClogFilterAdditionalFunction(ePoint &point)
{
    //если координаты точек выходят за границы - это просто игнорируется
    if(point.x >= int(column) || point.y >= int(row) )
        return;
    //Выбор режима - MediPix or TimePix
    if(pPixFilterPanel->isMediPix())
        arrayOrigin[point.x][point.y] = arrayOrigin[point.x][point.y] + 1;
    else
    {
        if(pSettings != nullptr && pSettings->value("GeneralCalibration/apply").toBool())
        {
            double A = (pSettings->value("GeneralCalibration/A").toDouble());
            double B = (pSettings->value("GeneralCalibration/B").toDouble());
            double C = (pSettings->value("GeneralCalibration/C").toDouble());
            double T = (pSettings->value("GeneralCalibration/T").toDouble());

            double parA = A;
            double parB = B - point.tot - A * T;
            double parC = point.tot * T - B * T - C;

            point.tot = ( -parB + ( qSqrt(parB * parB - 4 * parA * parC)) ) / (2 * parA);
        }
        arrayOrigin[point.x][point.y] = arrayOrigin[point.x][point.y] + point.tot;
    }
}
void Viewer::imageSettingsForImage(QImage &image)
{
    if(pSettings != nullptr)
    {
        if(pSettings->value("SettingsImage/MasquradingGroupBox").toBool())
        {
            //рисуем маскированые пиксели выбраным цветом
            for (size_t  x = 0; x < column; ++x)
                for (size_t  y = 0; y < row; ++y)
                    if(arrayMask[x][y] > 0)
                        image.setPixelColor(int(x), int(y), QColor(pSettings->value("SettingsImage/maskColor").toString()));
            //удаляем массив для маскирования пикселей
            for (size_t  i = 0; i < column; ++i)
                delete[] arrayMask[i];
            delete[] arrayMask;
            arrayMask = nullptr;
        }
    }
}

void Viewer::calibrationSettingsForArray()
{
    if(pSettings != nullptr)
    {
//        pSettings->beginGroup("SettingsImage");

//        //если в настройка включено рисование рамки то рисуем её
//        if(pSettings->value("FrameGroupBox").toBool())
//            createFrameInArray();

//        //если в настройка включено маскирование пискелей то маскируем их
//        if(pSettings->value("MasquradingGroupBox").toBool())
//            createMaskInArray();

//        pSettings->endGroup();
    }



}
void Viewer::imageSettingsForArray()
{
    if(pSettings != nullptr)
    {
        //если в настройка включено рисование рамки то рисуем её
        if(pSettings->value("SettingsImage/FrameGroupBox").toBool())
            createFrameInArray();
        //если в настройка включено маскирование пискелей то маскируем их
        if(pSettings->value("SettingsImage/MasquradingGroupBox").toBool())
            createMaskInArray();
    }
}
void Viewer::createFrameInArray()
{
    if(pSettings != nullptr)
    {
        pSettings->beginGroup("SettingsImage");

        size_t  width = size_t(pSettings->value("frameWidth").toInt());
        if(width > column || width > row) width = 0;

        int value = pSettings->value("frameValue").toInt();

        //верх
        for (size_t  x = 0; x < column; ++x)
            for (size_t  y = 0; y < width; ++y)
                arrayOrigin[x][y] = value;
        //низ
        for (size_t  x = 0; x < column; ++x)
            for (size_t  y = row - 1; y >= row - width; --y)
                arrayOrigin[x][y] = value;
        //лево
        for (size_t  y = 0; y < row; ++y)
            for (size_t  x = 0; x < width; ++x)
                arrayOrigin[x][y] = value;
        //право
        for (size_t  y = 0; y < row; ++y)
            for (size_t  x = column - 1; x >= column - width; --x)
                arrayOrigin[x][y] = value;

        pSettings->endGroup();

    }
}
void Viewer::createMaskInArray()
{
    if(pSettings != nullptr)
    {
        //выделяем память для массива маскирования пикселей, удаляем его в функции imageSettingsForImage
        arrayMask = new double *[column];
        for (size_t  i = 0; i < column; ++i)
            arrayMask[i] = new double[row];
        //обнуляем его
        for (size_t  x = 0; x < column; ++x)
            for (size_t  y = 0; y < row; ++y)
                arrayMask[x][y] = 0;

        pSettings->beginGroup("SettingsImage")        ;

        int value       = pSettings->value("maskValue").toInt();
        int newValue    = pSettings->value("maskNewValue").toInt();
        bool after      = pSettings->value("maskAfter").toBool();

        pSettings->endGroup();

        //пробегаемся по всему массиву
        for (size_t  x = 0; x < column; ++x)
            for (size_t  y = 0; y < row; ++y)
            {
                if(after)
                {
                    if(arrayOrigin[x][y] >= value)
                    {
                        arrayOrigin[x][y] = newValue;
                        arrayMask[x][y] = 1;
                    }
                }
                else if(arrayOrigin[x][y] <= value)
                {
                    arrayOrigin[x][y] = newValue;
                    arrayMask[x][y] = 1;
                }
            }
    }
}

void Viewer::createButtonMenu()
{
    pMenuFile = new QMenu;

    pMenuFile->addAction("pix. && filter panel", this,
                         SLOT(slotPFP()));
    pMenuFile->addAction("data pnael", this,
                         SLOT(slotDP()));
    pMenuFile->addAction("buttons panel", this,
                         SLOT(slotBP()));
    pMenuFile->addAction("inversion", this,
                         SLOT(slotI()));
    pMenuFile->addSeparator();
    pMenuFile->addAction("separate window", this,
                         SLOT(slotSW()));

    ui->button_settings->setMenu(pMenuFile);

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
    unsigned int lenght = sizeof(action_array) / sizeof (action_array[0]) - 1; // -1 - для последнего пункта не надо
    for (unsigned int i = 0; i < lenght; ++i){
            pMenuFile->actions()[int(i)]->setCheckable(true);
            pMenuFile->actions()[int(i)]->setChecked(true);
    }
}

void Viewer::createButtonPanel()
{
    if(pViewerButtonPanel != nullptr)
        return;

    pViewerButtonPanel = new Viewer_Button_Panel;
    //повороты
    connect(pViewerButtonPanel, SIGNAL(signalRotatePlus()), this, SLOT(slotRotatePlus()));
    connect(pViewerButtonPanel, SIGNAL(signalRotateMinus()), this, SLOT(slotRotateMinus()));
//    connect(pViewerButtonPanel, SIGNAL(valueChanged(double)), this, SLOT(slotRotatePlus()));
    //зеркальное отражение
    connect(pViewerButtonPanel, SIGNAL(signalMirrorHorizontal()), this, SLOT(slotMirrorHorizontal()));
    connect(pViewerButtonPanel, SIGNAL(signalMirrorVertical()), this, SLOT(slotMirrorVertical()));
    //сброс трансформации
    connect(pViewerButtonPanel, SIGNAL(signalResetTransform()), this, SLOT(slotResetTransform()));
    //масштаб
    connect(pViewerButtonPanel, SIGNAL(signalScaledPlus()), this, SLOT(slotScaledPlus()));
    connect(pViewerButtonPanel, SIGNAL(signalScaledMinus()), this, SLOT(slotScaledMinus()));
    //сохранение в bmp и txt
    connect(pViewerButtonPanel, SIGNAL(signalSaveToBmp()), this, SLOT(slotSaveBMP()));
    connect(pViewerButtonPanel, SIGNAL(signalSaveToTxt()), this, SLOT(slotSaveTXT()));

    this->layout()->addWidget(pViewerButtonPanel);
}

void Viewer::createDataPanel()
{
    if(pViewerDataPanel != nullptr)
        return;

    pViewerDataPanel = new Viewer_Data_Panel;
    if(arrayOrigin != nullptr)
        connect(eventFilterScene, SIGNAL(signalMousePos(QPointF)), this, SLOT(slotViewPosition(QPointF)));

    this->layout()->addWidget(pViewerDataPanel);
}

void Viewer::createPixFilterPanel()
{
    if(pPixFilterPanel != nullptr)
        return;

    pPixFilterPanel = new Pix_Filter_Panel;

    ui->layout_graphicView_and_Pix_filter_panel->addWidget(pPixFilterPanel);
    ui->layout_graphicView_and_Pix_filter_panel->setStretch(0,1);

    connectPixFilterPanel();
}

void Viewer::slotSetImageFile(QString file)
{
    setImageFile(file);
}
// !!!
void Viewer::slotMoveRectFromKey()
{
    if(!pPixFilterPanel)
        return;

    QGraphicsRectItem* rectItem = static_cast<QGraphicsRectItem*>(currentScene->items().at(0));

    rectItem->setPos(0, 0);
    rectItem->setRect(pPixFilterPanel->getX(), pPixFilterPanel->getY(),
                      pPixFilterPanel->getWidth(), pPixFilterPanel->getHeight());
}
void Viewer::slotCreateRectItem(QGraphicsRectItem * item)
{
    itemRect = item;
}
void Viewer::slotApplyClogFilter()
{
    QImage image(int(column), int(row), QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::white);

    applyClogFilter(image);

    setImage(image);
}
void Viewer::slotRepaint()
{
    double max = findMaxInArrayOrigin();
    double min = findMinInArrayOrigin();
    //наполнение объекта QImage
    for (size_t  x = 0; x < column; ++x)
        for (size_t  y = 0; y < row; ++y) {
            double value = convert(arrayOrigin[x][y], \
                                            min, \
                                            max, \
                                            double(0), \
                                            double(255) );
            QColor color(qRound(value), qRound(value), qRound(value));
            imageOrigin.setPixelColor(int(x), int(y), color);
        }
    slotInversionCheckBox(ui->inversion->checkState());
}

void Viewer::slotPFP()
{
    if(fType == fileType::CLOG)
    {
        QMessageBox::critical(this, "Error", "You cannot disable this panel when viewing this type of file!");
        return;
    }

    if(action_array[PIX_AND_FILTER_PANEL])
    {
        objectDelete(pPixFilterPanel);
        pPixFilterPanel = nullptr;
        if(itemRect)
        {
            currentScene->removeItem(itemRect);
            itemRect = nullptr;
        }
        ui->graphicsView->unsetCursor();
    }
    else
        createPixFilterPanel();

    action_array[PIX_AND_FILTER_PANEL] = !action_array[PIX_AND_FILTER_PANEL];
}
void Viewer::slotDP()
{
    if(action_array[DATA_PANEL])
    {
        objectDelete(pViewerDataPanel);
        pViewerDataPanel = nullptr;
    }
    else
        createDataPanel();

    action_array[DATA_PANEL] = !action_array[DATA_PANEL];
}
void Viewer::slotBP()
{
    if(action_array[BUTTONS_PANEL])
    {
        objectDelete(pViewerButtonPanel);
        pViewerButtonPanel = nullptr;
    }
    else
        createButtonPanel();

    action_array[BUTTONS_PANEL] = !action_array[BUTTONS_PANEL];
}
void Viewer::slotI()
{

    ui->inversion->setVisible(!action_array[INVERSION]);
    action_array[INVERSION] = !action_array[INVERSION];
}
void Viewer::slotSW()
{   
    Viewer* SW = new Viewer;
    SW->setWindowTitle(filePath);
    SW->setSettings(*pSettings);

    if(fType == fileType::TXT){
        SW->setImageFile(filePath);
        for (size_t  x = 0; x < column; ++x)
            for (size_t  y = 0; y < row; ++y)
                SW->arrayOrigin[x][y] = arrayOrigin[x][y];

        SW->slotRepaint();
    }
    else if(fType == fileType::CLOG)
    {
        SW->setImageFile(filePath);
//        SW->pPixFilterPanel = pPixFilterPanel;
        SW->slotApplyClogFilter();

//        for (size_t  x = 0; x < column; ++x)
//            for (size_t  y = 0; y < row; ++y)
//                SW->arrayOrigin[x][y] = arrayOrigin[x][y];

//        SW->slotRepaint();
    }
    else{
        QTemporaryFile tmpFile;
        QString pref = ".txt";
        tmpFile.open();
        QFile file(tmpFile.fileName()+pref);
        tmpFile.close();

        QTextStream writeStrime(&file);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        for (size_t x = 0; x < row; ++x)
        {
            for (size_t y = 0; y < column; ++y) {
                if(y != 0)
                    writeStrime << " ";
                writeStrime << QString::number(arrayOrigin[y][x]);
            }
            writeStrime << "\n";
            writeStrime.flush();
        }

        SW->setImageFile(file.fileName());

        file.close();
    }
    SW->show();
}

void Viewer::slotViewSelectionMovePos(QPoint point)
{
    disconnectPixFilterPanelSelectionBox();

    if(pPixFilterPanel)
    {
        pPixFilterPanel->setX(point.x());
        pPixFilterPanel->setY(point.y());
    }

    connectPixFilterPanelSelectionBox();
}

void Viewer::slotDrawPoint(QPointF point)
{
    if(!pPixFilterPanel)
        return;

    int x = int(point.x());
    int y = int(point.y());

    if(x >= 0 && x < int(column) && y >= 0 && y < int(row))
    {
        imageOrigin.setPixelColor(x, y, pPixFilterPanel->getPenColor());
        slotInversionCheckBox(ui->inversion->checkState());
        arrayOrigin[x][y] = pPixFilterPanel->getPenValue();
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

    if(!pPixFilterPanel)
        return;

    pPixFilterPanel->setX(x);
    pPixFilterPanel->setY(y);
    pPixFilterPanel->setWidth(width);
    pPixFilterPanel->setHeight(height);
}
void Viewer::slotFinishSelection()
{
    ui->graphicsView->unsetCursor();
    setReadOnlyDataPanelSelection(false);
    pPixFilterPanel->finishSelection();
    pPixFilterPanel->setButtonCutDisable(false);
}
QImage Viewer::createArrayImage(const QString& fileName)
{
    ListData data(fileName);

    column  = data.column;
    row     = data.row;

    QImage image(int(column), int(row), QImage::Format_ARGB32_Premultiplied);

    if(column == 0 || row == 0 || column-1 > 65535 || row-1 > 65535)
        return QImage(QSize(0,0),QImage::Format_Invalid);

    //выделяем память для основного массива
    arrayOrigin = new double *[column];
    for (size_t  i = 0; i < column; ++i)
        arrayOrigin[i] = new double[row];

    //Заполнение матрицы данными из файла
    int iterrator = 0;
    double value = 0;
    for(size_t  y = 0; y < row; ++y)
    {
        for(size_t  x = 0; x < column; ++x) {
            value =  data.list.at(iterrator++);
            arrayOrigin[x][y] = value;
        }
    }

    //проверяем настройки для картинки такие как рамка и маскирование пикселей
    imageSettingsForArray();

    double max = findMaxInArrayOrigin();
    double min = findMinInArrayOrigin();

    //наполнение объекта QImage
    for (size_t  x = 0; x < column; ++x)
        for (size_t  y = 0; y < row; ++y) {
            double value = convert(arrayOrigin[x][y], \
                                            min, \
                                            max, \
                                            double(0), \
                                            double(255) );

            QColor color(qRound(value), qRound(value), qRound(value));
            image.setPixelColor(int(x), int(y), color);
        }

    imageSettingsForImage(image);

    return image;
}

double Viewer::findMaxInArrayOrigin()
{
    double max = 0;
    double value = 0;

    for (size_t y = 0; y < row; ++y)
        for (size_t x = 0; x < column; ++x) {
            value = arrayOrigin[x][y];
            if(max < value) max = value;
        }

    return max;
}

double Viewer::findMinInArrayOrigin()
{
    double min = std::numeric_limits<double>::max();
    double value = 0;

    for (size_t y = 0; y < row; ++y)
        for (size_t x = 0; x < column; ++x) {
            value = arrayOrigin[x][y];
            if(min > value) min = value;
        }

    return min;
}
double Viewer::convert(double value, double From1, double From2, double To1, double To2)
{
    if( (int(From1) == int(From2)) ||  (int(To1) == int(To2)))
        return 0.0;

    return (value-From1)/(From2-From1)*(To2-To1)+To1;
}
void Viewer::incorrectFile()
{
    setEnableButtonPanel(false);
    selectFile();
    if(pViewerDataPanel)
    {
        //Отображение на панели координаты курсора (x,y) относительно graphicsView
        disconnect(eventFilterScene, SIGNAL(signalMousePos(QPointF)), this, SLOT(slotViewPosition(QPointF)));
    }
    //Отображение на панели данных о выделении(x,y,width,height)
    disconnect(eventFilterScene, SIGNAL(siganlRect(QRect)), this, SLOT(slotViewSelectionPos(QRect)));
    //Отображение на панели координат выделения при перемещении(x,y)
    disconnect(eventFilterScene, SIGNAL(signalRectMove(QPoint)), this, SLOT(slotViewSelectionMovePos(QPoint)));
    //Изображение курсора - стрелка, выключение кнопки edit
    disconnect(eventFilterScene, SIGNAL(signalRelease()), this, SLOT(slotFinishSelection()));

    disconnect(eventFilterScene, SIGNAL(signalCreateRectItem(QGraphicsRectItem*)), this, SLOT(slotCreateRectItem(QGraphicsRectItem*)));
    disconnect(eventFilterScene, SIGNAL(signalDrawPoint(QPointF)), this, SLOT(slotDrawPoint(QPointF)));

    if(pViewerDataPanel)
        pViewerDataPanel->setAllData(0,0,0.0,0,0);
    clearArrayOrigin();
}
QImage Viewer::getImage()
{
    return imageOrigin;
}
QImage Viewer::getImageInversion()
{
    QImage image(imageOrigin);
    image.invertPixels();
    return image;
}

void Viewer::resetTransform()
{
    ui->graphicsView->resetTransform();
}

QWidget *Viewer::viewport()
{
    return ui->graphicsView->viewport();
}

QGraphicsScene *Viewer::getScene()
{
    if(currentScene)
        return currentScene;
    else
        return &defaultScene;
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
        if(itemRect)
        {
            currentScene->removeItem(itemRect);
            itemRect = nullptr;
        }
        ui->graphicsView->setCursor(QCursor(Qt::CrossCursor));
    }
    if(!state && itemRect == nullptr)
    {
        ui->graphicsView->unsetCursor();
        setEnableDataPanelSelection(false);
    }
}

void Viewer::slotPen(bool value)
{
    if(value){
       ui->graphicsView->setCursor(QCursor(QPixmap(":/pen").scaled(24,24, Qt::KeepAspectRatio, Qt::SmoothTransformation), X_HOT ,Y_HOT));
    }
    else{
       ui->graphicsView->unsetCursor();
    }
}
void Viewer::slotInversionCheckBox(int state)
{
    switch (state)
    {
    case (Qt::Unchecked):
        itemForeground->setPixmap(QPixmap::fromImage(imageOrigin));
        break;
    case (Qt::Checked):
        QImage inversion(imageOrigin);
        inversion.invertPixels(QImage::InvertRgb);
        itemForeground->setPixmap(QPixmap::fromImage(inversion));
        break;
    }
}
void Viewer::slotCut()
{
    if(!itemRect)
    {
        QMessageBox::critical(this, "Error", "Please, select an area!");
        return;
    }


    size_t column  = size_t(pPixFilterPanel->getWidth());
    size_t row     = size_t(pPixFilterPanel->getHeight());

    QImage image(int(column), int(row), QImage::Format_ARGB32_Premultiplied);

    //Временный массив для данных преобразованного диапазона
    double** array;

    //выделяем память для временного массива
    array = new double *[column];
    for (size_t i = 0; i < column; ++i)
        array[i] = new double[row];

    //Заполнение временного массива данными из выделенной области
    double value = 0;
    if(!pPixFilterPanel)
    {
        qDebug() << __FUNCTION__ << " pPixFilterPanel=nullptr";
        exit(1);
    }
    for (int x = pPixFilterPanel->getX(), tmpX = 0; tmpX < int(column); ++x, ++tmpX)
        for (int y = pPixFilterPanel->getY(), tmpY = 0; tmpY < int(row); ++y, ++tmpY) {
            if( (x < 0) || (x >= int(this->column)) || (y < 0) || (y >= int(this->row)) )
                value = 0;
            else
                value = arrayOrigin[x][y];
            array[tmpX][tmpY] = value;
        }

    //очищаем основной массив и переписываем переменные column и row
    clearArrayOrigin();
    this->column= column;
    this->row   = row;

    //выделяем память для основного массива
    arrayOrigin = new double *[column];
    for (size_t i = 0; i < column; ++i)
        arrayOrigin[i] = new double[row];

    value = 0;
    //копируем временный массив в основной
    for (size_t x = 0; x < column; ++x)
        for (size_t y = 0; y < row; ++y) {
            value = array[x][y];
            arrayOrigin[x][y] = value;
        }

    double max = findMaxInArrayOrigin();
    double min = findMinInArrayOrigin();

    // преобразование диапазонов
    for (size_t x = 0; x < column; ++x)
        for (size_t y = 0; y < row; ++y) {
            double value = convert(arrayOrigin[x][y], \
                                            min, \
                                            max, \
                                            double(0), \
                                            double(255) );
            array[x][y] = value;
        }

    //наполнение объекта QImage
    for (size_t x = 0; x < column; ++x)
        for (size_t y = 0; y < row; ++y) {
            double value = array[x][y];
            QColor color(qRound(value), qRound(value), qRound(value));
            image.setPixelColor(int(x), int(y), color);
        }

    //удаление временного массива
    for (size_t i = 0; i < column; ++i)
        delete[] array[i];
    delete[] array;

    setImage(image);

    if(fType == fileType::CLOG && pPixFilterPanel)
        pPixFilterPanel->setTabEnable(Pix_Filter_Panel::CLOG_FILTER_TAB, false);

    fType = fileType::UNDEFINED;
}
void Viewer::slotRotatePlus()
{
//        ui->angle->setValue(ui->angle->value() + 90);
    size_t matrix_rang = row;
    for (size_t i = 0; i < row / 2; i++)
    {
        for (size_t j = i; j < row - 1 - i; j++)
        {
            double tmp = arrayOrigin[i][j];
            arrayOrigin[i][j] = arrayOrigin[j][matrix_rang - i - 1];
            arrayOrigin[j][matrix_rang - i - 1] = arrayOrigin[matrix_rang - i - 1][matrix_rang - j - 1];
            arrayOrigin[matrix_rang - i - 1][matrix_rang - j - 1] = arrayOrigin[matrix_rang - j - 1][i];
            arrayOrigin[matrix_rang - j - 1][i] = tmp;
        }
    }
    slotRepaint();
}
void Viewer::slotRotateMinus()
{
    size_t matrix_rang = row;
    for (size_t i = 0; i < row / 2; i++)
    {
        for (size_t j = i; j < row - 1 - i; j++)
        {
            double tmp = arrayOrigin[i][j];
            arrayOrigin[i][j] = arrayOrigin[matrix_rang - j - 1][i];
            arrayOrigin[matrix_rang - j - 1][i] = arrayOrigin[matrix_rang - i - 1][matrix_rang - j - 1];
            arrayOrigin[matrix_rang - i - 1][matrix_rang - j - 1] = arrayOrigin[j][matrix_rang - i - 1];
            arrayOrigin[j][matrix_rang - i - 1] = tmp;
        }
    }
    //        ui->angle->setValue(ui->angle->value() - 90);
    slotRepaint();
}

void Viewer::slotMirrorHorizontal()
{
    for (size_t i = 0; i < row / 2; i++)
                std::swap(arrayOrigin[i], arrayOrigin[row - i - 1]);

    slotRepaint();
}
void Viewer::slotMirrorVertical()
{
    for (size_t i = 0; i < row; i++)
    {
        for (size_t j = 0; j < row / 2; j++)
                    std::swap(arrayOrigin[i][j], arrayOrigin[i][row - j - 1]);
    }

    slotRepaint();
}
void Viewer::slotResetTransform()
{
    ui->graphicsView->resetTransform();
    ui->graphicsView->fitInView(imageOrigin.rect(), Qt::KeepAspectRatio);

//    angle = 0;
//    ui->angle->setValue(0);
}
void Viewer::slotViewPosition(QPointF pos)
{
    if(pViewerDataPanel == nullptr)
        return;

    int width = int(currentScene->sceneRect().width());
    int height = int(currentScene->sceneRect().height());

    if(pos.x() < 0 || pos.x() > width || pos.y() < 0 || pos.y() > height)
    {
        pViewerDataPanel->setX(0);
        pViewerDataPanel->setY(0);
        pViewerDataPanel->setAllData(0.0);
    }
    else if(arrayOrigin != nullptr)
    {
        int x = int(pos.x());
        int y = int(pos.y());

        if( (x < int(column)) && (y < int(row)) ){
            double data = arrayOrigin[x][y];
            pViewerDataPanel->setX(x);
            pViewerDataPanel->setY(y);
            pViewerDataPanel->setAllData(data);
        }
    }
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
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    ("Save file"),
                                                    QDir::rootPath(),
                                                    "Images (*.bmp);;All files (*.*)");


    QImage image(int(column), int(row), QImage::Format_RGB32);

    if(ui->inversion->isChecked())
        image = this->getImageInversion();
    else
        image = this->getImage();

    image.save(fileName, "BMP");
}

void Viewer::slotSaveTXT()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    ("Save file"),
                                                    QDir::rootPath(),
                                                    "TXT (*.txt);;All files (*.*)");

    QFile file(fileName);
    QTextStream writeStrime(&file);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    for (size_t x = 0; x < row; ++x)
    {
        for (size_t y = 0; y < column; ++y) {
            if(y != 0)
                writeStrime << " ";
            writeStrime << QString::number(arrayOrigin[y][x]);
        }
        writeStrime << "\n";
        writeStrime.flush();
    }
    file.close();
}
Viewer::~Viewer()
{
    delete ui;
    objectDelete(pMenuFile);
    clearArrayOrigin();
    objectDelete(eventFilterScene);
}

template<typename T>
void Viewer::objectDelete(T* obj)
{
    if(obj != nullptr)
        delete obj;
}
