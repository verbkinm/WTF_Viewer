#include "chartview.h"
#include <QtGui/QMouseEvent>

#include <QDebug>

ChartView::ChartView(QWidget *parent) :
    QChartView(parent)
{
    setRubberBand(QChartView::RectangleRubberBand);
}

void ChartView::setRangeDefault(const RangeAxis &rangeX, const RangeAxis &rangeY)
{
    _rangeX = rangeX;
    _rangeY = rangeY;
}

void ChartView::slotResetZoomAndPosition()
{
    chart()->zoomReset();
    chart()->axes(Qt::Horizontal).first()->setRange(_rangeX.min(), _rangeX.max());
    chart()->axes(Qt::Vertical).first()->setRange(_rangeY.min(), _rangeY.max());
}
void ChartView::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton)
        _lastMousePos = event->pos();

    QChartView::mousePressEvent(event);
}

void ChartView::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::RightButton)
    {
        auto dPos = event->pos() - _lastMousePos;
        chart()->scroll(-dPos.x(), dPos.y());

        _lastMousePos = event->pos();
        event->accept();
    }
    emit signalMousePosition(chart()->mapToValue(event->pos()));

    QChartView::mouseMoveEvent(event);
}

void ChartView::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton)
        return;

    QChartView::mouseReleaseEvent(event);
}

void ChartView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
        case Qt::Key_Plus:
            chart()->zoom(0.51);
            chart()->zoomIn();
            break;
        case Qt::Key_Minus:
            chart()->zoom(1.95);
            chart()->zoomOut();
            break;
        case Qt::Key_Left:
            chart()->scroll(-10, 0);
            break;
        case Qt::Key_Right:
            chart()->scroll(10, 0);
            break;
        case Qt::Key_Up:
            chart()->scroll(0, 10);
            break;
        case Qt::Key_Down:
            chart()->scroll(0, -10);
            break;
        case Qt::Key_Escape:
            slotResetZoomAndPosition();
            break;
        default:
            QGraphicsView::keyPressEvent(event);
            break;
    }
}

void ChartView::wheelEvent(QWheelEvent *event)
{
    if(event->angleDelta().y() > 0)
    {
        chart()->zoom(0.7);
        chart()->zoomIn();
    }
    else if(event->angleDelta().y() < 0)
    {
        chart()->zoom(1.5);
        chart()->zoomOut();
    }

    QChartView::wheelEvent(event);
}

bool ChartView::event(QEvent *ev)
{
    if(ev->type() == QEvent::Enter)
        this->setFocus();

    return QChartView::event(ev);
}
