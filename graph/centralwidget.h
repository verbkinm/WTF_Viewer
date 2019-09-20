#ifndef CENTRALWIDGET_H
#define CENTRALWIDGET_H

#include <QMainWindow>
#include <QLineSeries>
#include <QScatterSeries>
#include <QSplineSeries>
#include <QXYSeries>
#include <QValueAxis>
#include <QChart>
#include <QChartView>
#include <QValueAxis>
#include <QMenu>
#include <QMenuBar>
#include <QLabel>

#include <QHBoxLayout>

#include "chart.h"
#include "chartview.h"
#include "panelwidget.h"
#include "rangeaxis.h"

QT_CHARTS_USE_NAMESPACE

class CentralWidget : public QMainWindow
{
    Q_OBJECT
public:
    CentralWidget(QWidget *parent = nullptr);
    ~CentralWidget();

    void addSeries(std::map<double, double> &map, QString legendTitle = "",
                   QString axsisX_Title = "X", QString axsisY_Title = "Y");

    void setTitle(QString title);

    QString getTitle() const;
    QString getDataXType() const;
    QHBoxLayout getLayout() const;

private:
    QWidget _centralWidget;
    QLabel _statusBarWidget;

    ChartView _chartView;
    Chart _chart;

    QLineSeries _lineSeriesX;
    QLineSeries _lineSeriesY;

    QHBoxLayout _layout;

    PanelWidget _panelWidget;

    QValueAxis* _pAxisX;
    QValueAxis* _pAxisY;

    RangeAxis _rangeX, _rangeY;

    QMenu _menuFile, _menuView;

    void addSeries(std::map<double, double> &map, QXYSeries::SeriesType type = QXYSeries::SeriesTypeLine,
                   QString legendTitle = "", QString axsisX_Title = "X", QString axsisY_Title = "Y");

    //ось X и Y на всем полотне
    void setLinersXYDefault();
    void repaintXYAxes();

    void createAxes();
    void createMenu();
    QXYSeries *createSeriesAccordingType(QXYSeries::SeriesType &type);

    double findMaxX(QXYSeries *series);
    double findMinX(QXYSeries *series);
    double findMaxY(QXYSeries *series);
    double findMinY(QXYSeries *series);

    void connectPanelWidgetSignals();
    void fillSeriesOfPoints(std::map<double, double> &map, QXYSeries *series);

    void setMinAndMaxForXY(QXYSeries *series);
    void setRangeAndTitleForAxes(const QString &axsisX_Title, const QString &axsisY_Title);
    void setChartViewXYRange();
    void setSeriesProperty(QXYSeries *series);

private slots:
    void slotSetTheme(int);
    void slotSetLegentPosition(int);
    void slotSetTitle(QString);
    void slotAnimation(bool);
    void slotAntialiasing(bool);
    void slotSetTcickCountX(int);
    void slotSetTcickCountY(int);
    void slotSaveBMP();
    void slotReRange();
    void slotResetZoomAndPosition();
    void slotViewXYCoordinate(QPointF);
    void slotSetRubberMode(QChartView::RubberBand);
    void slotRangeXChanged(qreal, qreal);
    void slotRangeYChanged(qreal, qreal);
    void slotRangeXSet(qreal, qreal);
    void slotRangeYSet(qreal, qreal);
    void slotSeriesTypeChanged();

protected:
    virtual void closeEvent(QCloseEvent *);

signals:
    void signalCloseWindow(QObject *);
};

#endif // CENTRALWIDGET_H
