#ifndef VIEWER_H
#define VIEWER_H

#include <QWidget>
#include <QFile>
#include <QImage>
#include <QCloseEvent>
#include <QGraphicsScene>
#include <QSettings>
#include <QMenu>
#include <QFlags>

#include "../../eventfilter/fingerslide.h"
#include "../frames/frames.h"
#include "viewer_processor.h"
#include "viewer_button_panel.h"
#include "viewer_data_panel.h"
#include "pix_filter_panel.h"

namespace Ui {
class Viewer;
}

class Viewer : public QWidget
{
    Q_OBJECT

public:
    const static int CLOG_SIZE = 256;

    //For cursor with Pen
    const static int X_HOT = 2;
    const static int Y_HOT = 23;

    enum marker
    {
        NO_MARKERS = 0x0,
        BORDER = 0x1,
        MASKING = 0x2,
        GENERAL_CALIBRATION = 0x4
    };
    Q_DECLARE_FLAGS(Markers_Flags, marker)
    Viewer::Markers_Flags _markers;

    explicit Viewer(QWidget *parent = nullptr);
    ~Viewer();

    void        setSettings             (QSettings &settings);
    void        setScene                (QGraphicsScene* scene);
    void        setReadOnly             (bool value = true);
    //установка картинки на виджет из файла
    void        setImageFile            (QString fileName);


    QGraphicsScene* getScene();
    Frames*     getFrames();
    //получение Qimage из txt файла
    QImage      getImageFromTxtFile     (QString fileName);

    void        hideAllPanel            ();
    void        hideSettingsButton      (bool value = true);

private:
    // !
    QSettings*  pSettings = nullptr;
    // !
    //получение Qimage из clog файла
    QImage      getImageFromClogFile     (QString fileName);

    void        setImage                (QImage image);

    void        setSceneDefault         ();

    QImage      getImage                () const;
    QImage      getImageInversion       () const;

    void        resetTransform          ();
    QWidget*    getViewport() const;

    //объект сцены
    QGraphicsScene*                     currentScene = nullptr;
    QGraphicsScene                      defaultScene;

    size_t      getColumnFromFile       (QString fileName) const;
    size_t      getRowFromFile          (QString fileName) const;



signals:

private:
    Ui::Viewer *ui;

    // панели
    Viewer_Button_Panel *pViewerButtonPanel = nullptr;
    Viewer_Data_Panel *pViewerDataPanel = nullptr;
    Pix_Filter_Panel *pPixFilterPanel = nullptr;

    Viewer_Processor viewerProcessor;

    void createButtonPanel();
    void createDataPanel();
    void createPixFilterPanel();

    bool    readOnly = false;

    enum {PIX_AND_FILTER_PANEL, DATA_PANEL, BUTTONS_PANEL, INVERSION, SINGLE_WINDOW};
    /*
    [0 - pix. && filter pnael]
    [1 - data paNel]
    [2 - buttons panel]
    [3 - inversion]
    [4 - single window]
    видимость панелей
    */
    bool    action_array[5];

    enum class fileType {UNDEFINED, TXT, CLOG};

    //текущий тип файла
    fileType fType;
    QString  filePath;

//используется для нормального вращения QGraphicsView без сложных(для меня) модификаций QTransform
//    double angle = 0;

//объект, который хранит сам рисунок
    QImage      imageOrigin;
//фон
//    QImage      imageBackground;

//указатели на item`ы
    QGraphicsPixmapItem* itemForeground = nullptr;
    //рамка при выделении
    QGraphicsRectItem*   itemRect       = nullptr;

    // !
    //объект для хранения данных для работы с файлами clog
    Frames frames;
    // !
    //двумерный массив с данными из файла
    double**       arrayOrigin             = nullptr;
    //массив для нанесения маски из настроек Settings -Image
    double**       arrayMask               = nullptr;

    // !
    //переменные для хранения кол-ва строк и столбцов файла
    size_t column  = 0;
    size_t row     = 0;

    //фильтр событий для сцены и представления
    FingerSlide* eventFilterScene = nullptr;

    // ! TXT
    //возвращает рисунок из файла или QImage::Format_Invalid
    QImage      createArrayImage        (const QString& fileName);

    // !
    double      findMaxInArrayOrigin();
    double      findMinInArrayOrigin();
    // !
    //преобразование диапазонов
    double      convert                 (double value,
                                         double From1, double From2,
                                         double To1, double To2);

    //действия при не правильном файле
    void        incorrectFile           ();
    //вывести вместо изображения надпись - "Select file!"
    void        selectFile              ();
    //включени\отключение кнопок на панелях
    void        setEnableButtonPanel    (bool);
    // !
    //очистка динамического массива arrayOrigin
    void        clearArrayOrigin        ();
    //включени\отключение кнопок на панели Data
    void        setEnableDataPanelSelection(bool);
    //состояние readonly на панели Data
    void        setReadOnlyDataPanelSelection(bool);

    void        disconnectPixFilterPanelSelectionBox ();

    void connectPixFilterPanel();
    void connectPixFilterPanelSelectionBox();

    // !
    void applyClogFilter(QImage& image);
    void applyClogFilterAdditionalFunction(ePoint &point);
    // !
    void imageSettingsForArray();
    void imageSettingsForImage(QImage& image);

    // !
    void generalCalibrationSettingsForArray(ePoint &point);
    //создаёт рамку согласно настройкам
    void        createFrameInArray();
    //маскируем выбранные пиксели
    void        createMaskInArray();

    void showMarkers();
    void clearMarkers();

    QMenu*      pMenuFile               = nullptr;
    void        createButtonMenu        ();

    template <typename T>
    void        objectDelete(T* obj);

public slots:
    void        slotSetImageFile(QString file);
    //сохранинеие в bmp
    void        slotSaveBMP             ();
    //сохранинеие в txt
    void        slotSaveTXT             ();

private slots:
    //поворот
    void        slotRotatePlus();
    void        slotRotateMinus();
    // отражение по горизонтали
    void        slotMirrorHorizontal();
    void        slotMirrorVertical();
    //сброс трансформации
    void        slotResetTransform      ();
    //масштаб при нажатии на виджете кнопок + и -
    void        slotScaledPlus();
    void        slotScaledMinus();
    // масштаб колёсиком мышки
    void        slotScaleWheel          (int);

    //отрисовка imageOrigin в зависимости от значения checkBox'а Inversion
    void        slotInversionCheckBox   (int state);

    //позиция курсора и значение "пикселя" на сцене
    void        slotViewPosition        (QPointF);
    //позиция выделения
    void        slotViewSelectionPos    (QRect);
    //позиция выделения при перетаскивании мышкой
    void        slotViewSelectionMovePos(QPoint);
    void        slotDrawPoint           (QPointF point);

    //действия при нажатии кнопки на панели инструментов
    void        slotSelectionFrame      (bool state);
    void        slotPen                 (bool value);
    void        slotCut                 ();

    //действия при окончании выделения(отпускание кнопки мышки)
    void        slotFinishSelection     ();

    // изменение выделения с помощью спинбоксов на панели
    void        slotMoveRectFromKey     ();

    void        slotCreateRectItem(QGraphicsRectItem* item);

    void        slotApplyClogFilter     ();

    void        slotRepaint             ();

//    [0 - pix. && filter pnael]
//                [1 - data pnael]
//                [2 - buttons panel]
//                [3 - inversion]
//                [4 - single window]
    //действия для каждого пункта меню кнопки button_settings
    void        slotPFP(); //pix and filter panel
    void        slotDP(); // data panel
    void        slotBP(); // button panel
    void        slotI(); // inversion chekbox
    void        slotSW(); //siparate window
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Viewer::Markers_Flags)


#endif // VIEWER_H
