#ifndef PANELWIDGET_H
#define PANELWIDGET_H

#include <QWidget>
#include <QLineSeries>
#include <QSplineSeries>
#include <QScatterSeries>
#include <QSlider>
#include <QVXYModelMapper>

#include "customtablemodel.h"


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

    void addSeriesInList(QXYSeries* series);
    QList<QXYSeries*>* getSeriesList();
    int getSeriesType();

    void setTitle(QString title);
    void setRangeAxisX(qreal min, qreal max);
    void setRangeAxisY(qreal min, qreal max);

private:
    Ui::PanelWidget *ui;

    QList<QXYSeries*> _seriesList;
    QList<CustomTableModel*> _modelList;

    QXYSeries* _currentSeries;

    QVXYModelMapper _mapper;

    void createTableData();
    void recreateSeries(int numberSeries, int seriesType);
    QXYSeries *newSeries(int seriesType);
    //отключение всех необходимых элементов, если список графов пуст!
    void setDisableUnits(bool value);


signals:
    void signalChangeTheme(int);
    void signalSetLegendPosition(int);
    void signalSetTitile(QString);

    void signalAnimation(bool);
    void signalAntialiasing(bool);

    void signalTickCountChangeX(int);
    void signalTickCountChangeY(int);

    void signalSeriesDeleted();

    void signalRubberMode(QChartView::RubberBand);

    void signalAxisXRangeChanged(qreal, qreal);
    void signalAxisYRangeChanged(qreal, qreal);

    void signalSeriesTypeChanged();

private slots:
    void slotSetSeriesPropery(int value);
    void slotSetPointVisible(bool value);

    void slotSetSeriesColor();
    void slotSetSeriesType(int seriesType);
    void slotHideSeries(bool value);
    void slotSetSeriesPenWidth(int value);

    void slotDeleteSeries();

    void slotSaveToCSV();

    void on_actionSetRubberMode_triggered();

    void slotAxisXRangeChanged();
    void slotAxisYRangeChanged();
};

#endif // PANELWIDGET_H
