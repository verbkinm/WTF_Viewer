#ifndef FRAMES_H
#define FRAMES_H

#include "oneframe.h"

#include <limits>
#include <QObject>

class Frames : public QObject
{
    Q_OBJECT

public:
    Frames(QObject *parent = nullptr);

    void    addFrame        (int number);
    void    appendCluster   ();

    void    appendEPoint    (const ePoint& point);
    void    appendEPoint    (int x, int y, double tot);

    int getFrameCount() const;
    int getClusterCount(int frameNumber) const;
    int getClusterLenght(int frameNumber, int clusterNumber) const;
    int getEventCountInCluster(int frameNumber, int clusterNumber) const;

    const ePoint& getEPoint(int frameNumber, int clusterNumber, int eventNumber) const;

    void    setFile         (const QString& path);
    void    clear           ();
    const QList<OneFrame>&  getList() const;

    bool    isClusterInRange  (int clusterLength,
                             int clusterRangeBegin,
                             int clusterRangeEnd) const;
    bool    isTotInRange      (int frameNumber, int clusterNumber,
                             int totRangeBegin, int totRangeEnd) const;

    QList<ePoint> getListTotInRange(int frameNumber, int clusterNumber,
                                    int totRangeBegin, int totRangeEnd) const;

    QVector<int> getClustersLenghtList() const;
    //получение вектора кол-ва тотов с кластера заданного размера
    QVector<QPointF> getClusterVectorTot(const int& clusterLenght) const;
    //получение вектора кол-ва кластеров
    QVector<QPointF> getClusterVector() const;
    QVector<double> getTotLenghtList() const;

private:
    QList<OneFrame> list;

private slots:


signals:
    void signalFrameCreated(int);
    void signalFramesCreated();
};

#endif // FRAMES_H
