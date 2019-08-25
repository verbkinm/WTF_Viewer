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

QT_CHARTS_USE_NAMESPACE

class CentralWidget : public QMainWindow
{
    Q_OBJECT
public:
    CentralWidget(QWidget *parent = nullptr);
    ~CentralWidget();

    void addSeries(std::vector<QPointF> &pointVector, QXYSeries::SeriesType type = QXYSeries::SeriesTypeLine,
                   QString legendTitle = "", QString axsisX_Title = "X", QString axsisY_Title = "Y");
    void addSeries(std::vector<QPointF> &pointVector, QString legendTitle = "",
                   QString axsisX_Title = "X", QString axsisY_Title = "Y");

    void setTitle(QString title);

    QString getTitle() const;
    QString getDataXType() const;

    void createAxes();

private:
    QWidget centralWidget;
    QLabel statusBarWidget;

    ChartView chartView;
    Chart chart;

    QLineSeries lineSeriesX;
    QLineSeries lineSeriesY;

    QHBoxLayout layout;

    PanelWidget panelWidget;

    QValueAxis* pAxisX;
    QValueAxis* pAxisY;

    double maxX;
    double maxY;
    double minX;
    double minY;

    QMenu menuFile, menuView;

    //ось X и Y на всем полотне
    void setLinersXYDefault();

    void createMenu();

    double findMaxX(QXYSeries *);
    double findMinX(QXYSeries *);
    double findMaxY(QXYSeries *);
    double findMinY(QXYSeries *);

    void connectPanelWidgetSignals();
    QXYSeries *createSeriesAccordingType(QXYSeries::SeriesType &type);
    void fillSeriesOfPoints(std::vector<QPointF> &pointVector, QXYSeries *series);
    void setMinAndMaxForXY(QPointF &point);
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
