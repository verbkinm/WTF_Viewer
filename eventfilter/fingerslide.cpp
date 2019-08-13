#include "fingerslide.h"
#include "../viewer_widget/viewer/viewer.h"

#include <QDebug>
#include <QObject>
#include <QWidget>
#include <QAbstractScrollArea>
#include <QScrollBar>
#include <QApplication>
#include <QRectF>

#define SLIDE   1

FingerSlide::FingerSlide(QObject *parent) : QObject(parent)
{

}
bool FingerSlide::eventFilterScene(QObject* object, QEvent* event)
{
    QGraphicsScene *scene = static_cast<QGraphicsScene*>(object);
    QGraphicsView* graphView = scene->views().at(0);
//    QGraphicsView* graphView1 = scene->views().at(1);

// Движение
    if(event->type() == QEvent::GraphicsSceneMouseMove)
    {
        QGraphicsSceneMouseEvent* mevent = static_cast<QGraphicsSceneMouseEvent*>(event);
        emit signalMousePos(mevent->scenePos());

        if(scene->selectedItems().length() > 0 && QApplication::mouseButtons() == Qt::LeftButton)
        {
            QGraphicsRectItem* rectItem = static_cast<QGraphicsRectItem*>(scene->items().at(0));
            emit signalRectMove(rectItem->mapToScene(rectItem->rect().topLeft()).toPoint()) ;
        }

        if(QApplication::mouseButtons() == Qt::LeftButton && graphView->cursor() == Qt::CrossCursor)
        {
            QGraphicsRectItem* rectItem = static_cast<QGraphicsRectItem*>(scene->items().at(0));

            qreal x = mevent->scenePos().x();
            qreal y = mevent->scenePos().y();

            rectItem->setRect(QRectF(point, QSize(int(x - point.x()), int(y - point.y()))) );
            emit siganlRect(rectItem->rect().toRect());
        }
        if(graphView->cursor().hotSpot().x() == Viewer::X_HOT &&
           graphView->cursor().hotSpot().y() == Viewer::Y_HOT &&
           QApplication::mouseButtons() == Qt::LeftButton)
        {
            emit signalDrawPoint(mevent->scenePos());
        }
    }
//Нажатие
    if(event->type() == QEvent::GraphicsSceneMousePress)
    {
        QGraphicsSceneMouseEvent* mevent = static_cast<QGraphicsSceneMouseEvent*>(event);
        point = mevent->scenePos();

        //не перетаскивать рамку выделения, если мы рисуем карандашом
        if(scene->items().count() > 1 &&
            graphView->cursor().hotSpot().x() == Viewer::X_HOT &&
            graphView->cursor().hotSpot().y() == Viewer::Y_HOT)
        {
           QGraphicsRectItem* rectItem = static_cast<QGraphicsRectItem*>(scene->items().at(0));
           rectItem->setFlags(nullptr);
        }
        //если нажата левая кнопка мыши и при этом идет процесс рисования рамки
        if(QApplication::mouseButtons() == Qt::LeftButton && graphView->cursor() == Qt::CrossCursor)
        {
            QGraphicsRectItem* rectItem = scene->addRect(QRectF(QPointF(0,0), QSize(0,0)), \
                                                             QPen(QBrush(QColor(Qt::red)), 0));
//            rectItem->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
//            rectItem->setFlag(QGraphicsItem::ItemIsMovable, false);
//            rectItem->setFlag(QGraphicsItem::ItemIsSelectable, false);
            rectItem->setZValue(2);
            emit signalCreateRectItem(rectItem);
        }
        if(graphView->cursor().hotSpot().x() == Viewer::X_HOT &&
           graphView->cursor().hotSpot().y() == Viewer::Y_HOT)
        {
            QGraphicsSceneMouseEvent* mevent = static_cast<QGraphicsSceneMouseEvent*>(event);
            emit signalDrawPoint(mevent->scenePos());
        }
    }
//Отжатие
    if(event->type() == QEvent::GraphicsSceneMouseRelease)
    {
        //если есть рамка выделения
        if(graphView->scene()->items().length() > 1 )
        {
            QGraphicsRectItem* rectItem = static_cast<QGraphicsRectItem*>(scene->items().at(0));
            rectItem->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
            qreal x = rectItem->rect().x();
            qreal y = rectItem->rect().y();
            qreal width = rectItem->rect().width();
            qreal height = rectItem->rect().height();

/* при рисовании четырёх угольнника получаются отрицательные величины для ширины
и(или) высоты, чтобы исправить это - следующие 4 if'а */
            if(rectItem->rect().height() < 0 && rectItem->rect().width() < 0)
            {
                QRectF rect(int(x + width + 0.5), int(y - qAbs(height) + 0.5), qAbs(width), qAbs(height) );
                rectItem->setRect(rect);
            }
            else if(rectItem->rect().height() < 0 && rectItem->rect().width() > 0)
            {
                QRectF rect(int(x + 0.5), int(y - qAbs(height) + 0.5), qAbs(width), qAbs(height) );
                rectItem->setRect(rect);
            }
            else if(rectItem->rect().height() > 0 && rectItem->rect().width() < 0  )
            {
                QRectF rect(int(x + width + 0.5), int(y + 0.5), qAbs(width), height );
                rectItem->setRect(rect);
            }
            else if(rectItem->rect().height() > 0 && rectItem->rect().width() > 0  )
            {
                QRectF rect(int(x + 0.5), int(y + 0.5), width, height );
                rectItem->setRect(rect);
            }
//            дискретное установление выделения
            x = rectItem->pos().x();
            y = rectItem->pos().y();
            (x > 0) ? x = x + 0.5 : x = x - 0.5;
            (y > 0) ? y = y + 0.5 : y = y - 0.5;
            rectItem->setPos(int(x), int(y) );

            //если не идет процесс рисования
            if(graphView->cursor().hotSpot().x() != Viewer::X_HOT &&
               graphView->cursor().hotSpot().y() != Viewer::Y_HOT)
                emit signalRelease();
        }
    }

    return false;
}
bool FingerSlide::eventFilterViewport(QObject* object, QEvent* event)
{
    QGraphicsView* graphView = static_cast<QGraphicsView*>(object->parent());
//    QGraphicsScene *scene = graphView->scene();
// Колисо мышки
    if(event->type() == QEvent::Wheel)
        if(QApplication::keyboardModifiers() == Qt::ControlModifier)
        {
            QWheelEvent* wevent = static_cast<QWheelEvent*>(event);
            emit signalWheel(wevent->delta());
            return true;
        }
//Нажатие
    if(event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if(mouseEvent->button() == Qt::LeftButton)
        {
            preX = x = mouseEvent->x();
            preY = y = mouseEvent->y();
        }
//        else if(mouseEvent->button() == Qt::RightButton)
//            graphView->setCursor(QCursor(Qt::ArrowCursor));
    }
//Отжатие
//    if(event->type() == QEvent::MouseButtonRelease)
//    {
////        if(graphView->cursor() == Qt::CrossCursor)
////            graphView->setCursor(QCursor(Qt::ArrowCursor));
//    }
//Движение
    if(event->type() == QEvent::MouseMove)
    {
//        if(QApplication::mouseButtons() == Qt::LeftButton && graphView->cursor() == Qt::CrossCursor)
//            return true;
        if(QApplication::mouseButtons() == Qt::LeftButton && \
                (graphView->scene()->selectedItems().length() < 2 ) && \
                graphView->cursor() != Qt::CrossCursor &&
                graphView->cursor().hotSpot().x() != Viewer::X_HOT &&
                graphView->cursor().hotSpot().y() != Viewer::Y_HOT)
        {
            slide(object, event);
        }
    }

    return false;
}
bool FingerSlide::eventFilter(QObject* object, QEvent* event)
{
//    qDebug() << "event filter" << object->objectName() << event->type();

    if(object->objectName() == "scene")
        return eventFilterScene(object, event);
    else if(object->objectName() == "viewport")
        return eventFilterViewport(object, event);

    return false;
}
void FingerSlide::slide(QObject *object, QEvent *event)
{
    QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

    QAbstractScrollArea* pScrollArea = static_cast<QAbstractScrollArea*>(object->parent());
    QScrollBar* pScroll = nullptr;

    int differenceX, differenceY;

    preX = x;
    preY = y;


//горизонтальный прокрутка
    if(x > mouseEvent->x()){
        differenceX = mouseEvent->x() - x;
        x = mouseEvent->x();
        pScroll = pScrollArea->horizontalScrollBar();
        pScroll->setValue(pScroll->value() + abs(differenceX) );
    }
    if(x < mouseEvent->x()){
        differenceX = mouseEvent->x() - x;
        x = mouseEvent->x();
        pScroll = pScrollArea->horizontalScrollBar();
        pScroll->setValue(pScroll->value() - abs(differenceX) );
    }
//вертикальная прокрутка
    if(y > mouseEvent->y()){
        differenceY = mouseEvent->y() - y;
        y = mouseEvent->y();
        pScroll = pScrollArea->verticalScrollBar();
        pScroll->setValue(pScroll->value() + abs(differenceY) );
    }
    if(y < mouseEvent->y()){
        differenceY = mouseEvent->y() - y;
        y = mouseEvent->y();
        pScroll = pScrollArea->verticalScrollBar();
        pScroll->setValue(pScroll->value() - abs(differenceY) );
    }
}
FingerSlide::~FingerSlide()
{

}
