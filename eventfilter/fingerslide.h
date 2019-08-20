#ifndef FINGERSLIDE_H
#define FINGERSLIDE_H

#include <QGraphicsRectItem>
#include <QGraphicsView>
#include <QEvent>

class FingerSlide : public QObject
{
    Q_OBJECT
public:
    FingerSlide(QObject *parent = nullptr);
    ~FingerSlide();

private:
//координаты курсора
    int _x, _y;
    int _preX, _preY;
    int _stepX, _stepY;
    QGraphicsView *_pGraphView;
    QGraphicsScene *_pScene;
    QPointF _point, _rectPoint;

    QObject* _pObject;
    QEvent* _pEvent;

    bool isDrawingPen();

    void sceneMovement();
    void sceneMousePress();
    void sceneMouseRelease();
    void sceneDrawRect(QGraphicsRectItem* rectItem, qreal x, qreal y, qreal width, qreal height);

    void viewportSlide();
    void viewportSliding();
    void viewportSlidingHorizontal();
    void viewportSlidingVertical();
    bool viewportMouseWheel();
    void viewportSetPreXY();
    void viewportMovement();

    bool eventFilterScene();
    bool eventFilterViewport();

protected:
    virtual bool eventFilter(QObject* object, QEvent* event);

signals:
    void signalRelease();
    void signalWheel(int);
    void signalMousePos(QPointF);
    void siganlRect(QRect);
    void signalRectMove(QPoint);

    void signalCreateRectItem(QGraphicsRectItem*);

    void signalDrawPoint(QPointF);


};

#endif // FINGERSLIDE_H
