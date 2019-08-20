#ifndef VIEWER_H
#define VIEWER_H

#include <QGraphicsScene>

#include "../../eventfilter/fingerslide.h"

#include "viewer_processor/viewer_processor.h"
#include "viewer_processor/viewer_clog_processor.h"
#include "viewer_processor/viewer_txt_processor.h"

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
    //For cursor with Pen
    const static int X_HOT = 2;
    const static int Y_HOT = 23;

    explicit Viewer(QWidget *parent = nullptr);
    ~Viewer();

    void setSettings(std::shared_ptr<const QSettings>);
    void setScene(QGraphicsScene*);
    void setReadOnly(bool);
    void setImageFile(const QString &);

    QGraphicsScene* getScenePtr();
    std::pair<const Frames &, bool> getFrames() const;

    void hideAllPanel();
    void hideSettingsButton(bool);

private:
    Ui::Viewer *ui;
    std::shared_ptr<Viewer_Processor> _spViewerProcessor; // обработчик данных
    // панели
    std::unique_ptr<Viewer_Button_Panel> _spViewerButtonPanel;
    std::unique_ptr<Viewer_Data_Panel> _spViewerDataPanel;
    std::unique_ptr<Pix_Filter_Panel> _spPixFilterPanel;

    std::shared_ptr<const QSettings> _spSettings;
    bool _readOnly;

    /*
    [0 - pix. && filter pnael]
    [1 - data paNel]
    [2 - buttons panel]
    [3 - inversion]
    [4 - single window]
    видимость панелей
    */
    enum
    {
        PIX_AND_FILTER_PANEL,
        DATA_PANEL,
        BUTTONS_PANEL,
        INVERSION,
        SINGLE_WINDOW
    };

    std::array<bool, 5>  _state_of_the_menu_items;
    QString _filePath;
    QImage _currentImage;
    QGraphicsPixmapItem *_itemImage ;
    QGraphicsRectItem *_itemRect;
    FingerSlide *_pEventFilterScene;    //фильтр событий для сцены и представления
    std::unique_ptr<QMenu> _spMenuFile;
    //объект сцены
    QGraphicsScene* _pCurrentScene;
    QGraphicsScene _defaultScene;

    void setImage(const QImage &image);
    void setSceneDefault();

    void resetTransform();

    void createButtonPanel();
    void createDataPanel();
    void createPixFilterPanel();

    void incorrectFile();  //действия при не правильном файле
    void setEmptyImageOnViewerScene();     //вывести вместо изображения надпись - "Select file!"
    void setEnableButtonPanel(bool);    //включени\отключение кнопок на панелях
    void setEnableDataPanelSelection(bool);    //включени\отключение кнопок на панели Data
    void setReadOnlyDataPanelSelection(bool);    //состояние readonly на панели Data

    void disconnectPixFilterPanelSelectionBox ();
    void connectPixFilterPanel();
    void connectPixFilterPanelSelectionBox();
    void connectEventFilter();

    void showMarkers();

    void createButtonMenu();
    Filter_Clog createFilterFromPixFilterPanel();

public slots:
    void slotSetImageFile(QString file);
    void slotSaveBMP();
    void slotSaveTXT();

private slots:
    void slotRotatePlus();       //поворот по часовой стрелке
    void slotRotateMinus();      //поворот против часовой стрелке
    void slotMirrorHorizontal(); // отражение по горизонтали
    void slotMirrorVertical();   // отражение по горизонтали
    void slotResetTransform();   //сброс трансформации
    void slotScaledPlus();       //масштаб при нажатии на виджете кнопок +
    void slotScaledMinus();      //масштаб при нажатии на виджете кнопок -
    void slotScaleWheel(int);    // масштаб колёсиком мышки
    void slotInversionCheckBox(int);//отрисовка _currentImage в зависимости от значения checkBox'а Inversion
    void slotViewPosition(QPointF); //позиция курсора и значение "пикселя" на сцене
    void slotViewSelectionPos(QRect);//позиция выделения
    void slotViewSelectionMovePos(QPoint);//позиция выделения при перетаскивании мышкой
    void slotDrawPoint(QPointF); //рисование точек карандашем
    void slotSelectionFrame(bool);
    void slotPen(bool);
    void slotCut();
    void slotFinishSelection();    //действия при окончании выделения(отпускание кнопки мышки)
    void slotMoveRectFromKey();    // изменение выделения с помощью спинбоксов на панели
    void slotCreateRectItem(QGraphicsRectItem*);
    void slotApplyClogFilter();
    void slotRepaint();

    //действия для каждого пункта меню кнопки button_settings
    void slotPixAndFilterPanelMenuToggle();
    void slotDataPanelMenuToggle();
    void slotButtonPanelMenuToggle();
    void slotInversionMenuToggle();
    void slotSeparateWindowMenuToggle();
};
#endif // VIEWER_H
