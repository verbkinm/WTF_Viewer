#ifndef PANELWIDGET_H
#define PANELWIDGET_H

#include <QWidget>
#include <QLineSeries>
#include <QSplineSeries>
#include <QScatterSeries>

#include <QSlider>
#include <QVXYModelMapper>

#include "customtablemodel.h"
//#include <QXYSeries>


#include <QtCharts/QChartView>


QT_CHARTS_USE_NAMESPACE

namespace Ui {
class PanelWidget;
}

class PanelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PanelWidget(QWidget *parent = nullptr);
    ~PanelWidget();

    void    addSeriesList(QXYSeries* series);
    QList<QXYSeries*>* getSeriesList();
    int     getSeriesType();

    void    setTitle(QString title);

    void    setRangeAxisX(qreal, qreal);
    void    setRangeAxisY(qreal, qreal);


private:
    Ui::PanelWidget *ui;

    QList<QXYSeries*> seriesList;
    QList<CustomTableModel*> modelList;

    QXYSeries*      currentSeries = nullptr;

    QVXYModelMapper mapper;

    void createTableData();

    //отключение всех необходимых элементов, если список графов пуст!
    void    setDisableUnits(bool value);


signals:
    void    signalChangeTheme(int);
    void    signalSetLegendPosition(int);
    void    signalSetTitile(QString);

    void    signalAnimation(bool);
    void    signalAntialiasing(bool);

    void    signalTickCountChangeX(int);
    void    signalTickCountChangeY(int);

    void    signalSeriesDeleted();

    void    signalRubberMode(QChartView::RubberBand);

    void    signalAxisXRangeChanged(qreal, qreal);
    void    signalAxisYRangeChanged(qreal, qreal);

    void    signalSeriesTypeChange();


private slots:
    void    slotSetSeriesPropery(int value);
    void    slotSetPointVisible(bool value);

    void    slotSetSeriesColor  ();
    void    slotSetSeriesType   (int value);
    void    slotHideSeries      (bool value);
    void    slotSetSeriesPenWidth(int value);

    void    slotDeleteSeries();

    void    slotSaveToCSV();

    void    on_actionSetRubberMode_triggered();

    void    slotAxisXRangeChanged   ();
    void    slotAxisYRangeChanged   ();
};

#endif // PANELWIDGET_H
