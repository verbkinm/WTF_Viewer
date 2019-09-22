#ifndef CHARTVIEW_H
#define CHARTVIEW_H

#include <QtCharts/QChartView>
#include <QtWidgets/QRubberBand>

#include <QValueAxis>
#include "range.h"

QT_CHARTS_USE_NAMESPACE

class ChartView : public QChartView
{
    Q_OBJECT
public:
    ChartView(QWidget *parent = nullptr);

    void setRangeDefault(const Range<double> &rangeX, const Range<double> &rangeY);

public slots:
    void slotResetZoomAndPosition();

protected:
    bool vieportEvent(QEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
    bool event(QEvent* ev);

private:
    QPointF _lastMousePos;

    Range<double> _rangeX, _rangeY;

signals:
    void signalMousePosition(QPointF);
};

#endif
