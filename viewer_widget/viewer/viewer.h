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
#include <memory>

#include "../../eventfilter/fingerslide.h"
#include "../frames/frames.h"
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

    void setSettings(std::shared_ptr<QSettings>pShareSettings);
    void setScene(QGraphicsScene*);
    void setReadOnly(bool);
    void setImageFile(QString &);

    QGraphicsScene* getScene();

    void hideAllPanel();
    void hideSettingsButton(bool);

private:
    Ui::Viewer *ui;
    std::shared_ptr<Viewer_Processor> _pViewerProcessor; // обработчик данных
    // панели
    std::unique_ptr<Viewer_Button_Panel> _pViewerButtonPanel;
    std::unique_ptr<Viewer_Data_Panel> _pViewerDataPanel;
    std::unique_ptr<Pix_Filter_Panel> _pPixFilterPanel;

    std::shared_ptr<QSettings> _pSettings;
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
    QString  _filePath;
    QImage _currentImage;
    //указатели на item`ы
    QGraphicsPixmapItem* _itemForeground ;
    //рамка при выделении
    QGraphicsRectItem* _itemRect;
    //фильтр событий для сцены и представления
    std::unique_ptr<FingerSlide> _eventFilterScene;
    std::unique_ptr<QMenu> _pMenuFile;

    QWidget* getViewport() const;

    //объект сцены
    QGraphicsScene* _pCurrentScene;
    QGraphicsScene _defaultScene;

    void setImage(QImage image);

    void setSceneDefault();
    void resetTransform();

    void createButtonPanel();
    void createDataPanel();
    void createPixFilterPanel();

    void incorrectFile();  //действия при не правильном файле
    void selectFile();     //вывести вместо изображения надпись - "Select file!"
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

    template <typename T>
    void objectDelete(T* obj);

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
