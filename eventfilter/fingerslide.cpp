#include "fingerslide.h"
#include "../viewer_widget/viewer/viewer.h"

#include <QWidget>
#include <QAbstractScrollArea>
#include <QScrollBar>
#include <QApplication>
#include <QRectF>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QGraphicsItem>
#include <QGraphicsSceneMoveEvent>
#include <QGraphicsSceneMouseEvent>

FingerSlide::FingerSlide(QObject *parent) : QObject(parent)
{

}
bool FingerSlide::eventFilterScene()
{
    _pScene = static_cast<QGraphicsScene*>(_pObject);
    if(!_pScene->views().length())
        return false;
    _pGraphView = _pScene->views().at(0);

    sceneMovement();
    sceneMousePress();
    sceneMouseRelease();

    return false;
}
bool FingerSlide::eventFilterViewport()
{  
    if(viewportMouseWheel())
        return true;
    viewportSetPreXY();
    viewportMovement();

    return false;
}

bool FingerSlide::isDrawingPen()
{
    if(_pGraphView->cursor().hotSpot().x() == Viewer::X_HOT && _pGraphView->cursor().hotSpot().y() == Viewer::Y_HOT)
        return true;
    return false;
}

void FingerSlide::sceneMovement()
{
    if(_pEvent->type() != QEvent::GraphicsSceneMouseMove)
        return;

    QGraphicsSceneMouseEvent* pMouseEvent = static_cast<QGraphicsSceneMouseEvent*>(_pEvent);
    emit signalMousePos(pMouseEvent->scenePos());

    if(QApplication::mouseButtons() != Qt::LeftButton)
        return;

    if(_pScene->selectedItems().length())
    {
        QGraphicsRectItem* pRectItem = static_cast<QGraphicsRectItem*>(_pScene->items().at(0));
        emit signalRectMove(pRectItem->mapToScene(pRectItem->rect().topLeft()).toPoint()) ;
    }

    if(_pGraphView->cursor() == Qt::CrossCursor)
    {
        QGraphicsRectItem* rectItem = static_cast<QGraphicsRectItem*>(_pScene->items().at(0));
        qreal x = pMouseEvent->scenePos().x();
        qreal y = pMouseEvent->scenePos().y();
        rectItem->setRect(QRectF(_point, QSize(int(x - _point.x()), int(y - _point.y()))) );
        emit siganlRect(rectItem->rect().toRect());
    }
    if(isDrawingPen())
        emit signalDrawPoint(pMouseEvent->scenePos());
}

void FingerSlide::sceneMousePress()
{
    if(_pEvent->type() != QEvent::GraphicsSceneMousePress)
        return;

    QGraphicsSceneMouseEvent* mevent = static_cast<QGraphicsSceneMouseEvent*>(_pEvent);
    _point = mevent->scenePos();

    //не перетаскивать рамку выделения, если мы рисуем карандашом
    if(_pScene->items().count() > 1 && isDrawingPen())
        static_cast<QGraphicsRectItem*>(_pScene->items().at(0))->setFlags(nullptr);
    //если нажата левая кнопка мыши и при этом идет процесс рисования рамки
    if(QApplication::mouseButtons() == Qt::LeftButton && _pGraphView->cursor() == Qt::CrossCursor)
    {
        QGraphicsRectItem* rectItem = _pScene->addRect(QRectF(QPointF(0,0), QSize(0,0)), \
                                                       QPen(QBrush(QColor(Qt::red)), 0));
        rectItem->setZValue(2);
        emit signalCreateRectItem(rectItem);
    }
    if(isDrawingPen())
    {
        QGraphicsSceneMouseEvent* mevent = static_cast<QGraphicsSceneMouseEvent*>(_pEvent);
        emit signalDrawPoint(mevent->scenePos());
    }
}

void FingerSlide::sceneMouseRelease()
{
    if(_pEvent->type() != QEvent::GraphicsSceneMouseRelease || _pGraphView->scene()->items().length() == 1 || isDrawingPen())
        return;

    QGraphicsRectItem* rectItem = static_cast<QGraphicsRectItem*>(_pScene->items().at(0));
    rectItem->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
    qreal x = rectItem->rect().x();
    qreal y = rectItem->rect().y();
    qreal width = rectItem->rect().width();
    qreal height = rectItem->rect().height();

    sceneDrawRect(rectItem, x, y, width, height);

    // дискретное установление выделения
    x = rectItem->pos().x();
    y = rectItem->pos().y();
    (x > 0) ? x = x + 0.5 : x = x - 0.5;
    (y > 0) ? y = y + 0.5 : y = y - 0.5;
    rectItem->setPos(static_cast<int>(x), static_cast<int>(y) );

    emit signalRelease();
}

void FingerSlide::sceneDrawRect(QGraphicsRectItem *rectItem, qreal x, qreal y, qreal width, qreal height)
{
    /* при рисовании четырёх угольнника(снизу вверх) получаются отрицательные величины для ширины
    и(или) высоты, чтобы исправить это - следующие 4 if'а */
    if(rectItem->rect().height() < 0 && rectItem->rect().width() < 0)
    {
        QRectF rect(static_cast<int>(x + width + 0.5), static_cast<int>(y - qAbs(height) + 0.5), qAbs(width), qAbs(height) );
        rectItem->setRect(rect);
    }
    else if(rectItem->rect().height() < 0 && rectItem->rect().width() > 0)
    {
        QRectF rect(static_cast<int>(x + 0.5), static_cast<int>(y - qAbs(height) + 0.5), qAbs(width), qAbs(height) );
        rectItem->setRect(rect);
    }
    else if(rectItem->rect().height() > 0 && rectItem->rect().width() < 0  )
    {
        QRectF rect(static_cast<int>(x + width + 0.5), static_cast<int>(y + 0.5), qAbs(width), height );
        rectItem->setRect(rect);
    }
    else if(rectItem->rect().height() > 0 && rectItem->rect().width() > 0  )
    {
        QRectF rect(static_cast<int>(x + 0.5), static_cast<int>(y + 0.5), width, height );
        rectItem->setRect(rect);
    }
}
bool FingerSlide::eventFilter(QObject* object, QEvent* event)
{
    _pObject = object;
    _pEvent = event;

    if(object->objectName() == "scene")
        return eventFilterScene();
    else if(object->objectName() == "viewport")
        return eventFilterViewport();
    return false;
}
void FingerSlide::viewportSlide()
{
    _preX = _x;
    _preY = _y;
    viewportSliding();
}

void FingerSlide::viewportSliding()
{
    viewportSlidingHorizontal();
    viewportSlidingVertical();
}

void FingerSlide::viewportSlidingHorizontal()
{
    QAbstractScrollArea* pScrollArea = static_cast<QAbstractScrollArea*>(_pObject->parent());
    QScrollBar* pScroll = nullptr;
    QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(_pEvent);
    int differenceX;

    if(_x > mouseEvent->x())
    {
        differenceX = mouseEvent->x() - _x;
        _x = mouseEvent->x();
        pScroll = pScrollArea->horizontalScrollBar();
        pScroll->setValue(pScroll->value() + abs(differenceX) );
    }
    if(_x < mouseEvent->x())
    {
        differenceX = mouseEvent->x() - _x;
        _x = mouseEvent->x();
        pScroll = pScrollArea->horizontalScrollBar();
        pScroll->setValue(pScroll->value() - abs(differenceX) );
    }
}

void FingerSlide::viewportSlidingVertical()
{
    QAbstractScrollArea* pScrollArea = static_cast<QAbstractScrollArea*>(_pObject->parent());
    QScrollBar* pScroll = nullptr;
    QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(_pEvent);
    int differenceY;

    if(_y > mouseEvent->y())
    {
        differenceY = mouseEvent->y() - _y;
        _y = mouseEvent->y();
        pScroll = pScrollArea->verticalScrollBar();
        pScroll->setValue(pScroll->value() + abs(differenceY) );
    }
    if(_y < mouseEvent->y())
    {
        differenceY = mouseEvent->y() - _y;
        _y = mouseEvent->y();
        pScroll = pScrollArea->verticalScrollBar();
        pScroll->setValue(pScroll->value() - abs(differenceY) );
    }
}

bool FingerSlide::viewportMouseWheel()
{
    if(_pEvent->type() == QEvent::Wheel && QApplication::keyboardModifiers() == Qt::ControlModifier)
    {
        QWheelEvent* wevent = static_cast<QWheelEvent*>(_pEvent);
        emit signalWheel(wevent->delta());
        return true;
    }
    return  false;
}

void FingerSlide::viewportSetPreXY()
{
    if(_pEvent->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(_pEvent);
        if(mouseEvent->button() == Qt::LeftButton)
        {
            _preX = _x = mouseEvent->x();
            _preY = _y = mouseEvent->y();
        }
    }
}

void FingerSlide::viewportMovement()
{
    if(_pEvent->type() != QEvent::MouseMove)
        return;
    _pGraphView = static_cast<QGraphicsView*>(_pObject->parent());
    if(QApplication::mouseButtons() == Qt::LeftButton &&
            (_pGraphView->scene()->selectedItems().length() < 1 ) &&
            _pGraphView->cursor() != Qt::CrossCursor &&
            !isDrawingPen())
    {
        viewportSlide();
    }
}
FingerSlide::~FingerSlide()
{

}
