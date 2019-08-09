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

    void    addFrame        (const int& number);
    void    appendCluster   ();

    void    appendEPoint    (const ePoint& point);
    void    appendEPoint    (const int& x, const int& y, const double &tot);

    int getFrameCount() const;
    int getClusterCount(const int& frameNumber) const;
    int getClusterLenght(const int& frameNumber, const int& clusterNumber) const;
    int getEventCountInCluster(const int& frameNumber, const int& clusterNumber) const;

    const ePoint& getEPoint(const int& frameNumber, const int& clusterNumber, const int& eventNumber) const;

    void    setFile         (const QString& path);
    void    clear           ();
    const QList<OneFrame>&  getList() const;

    bool    clusterInRange  (const int& clusterLength,
                             const int& clusterRangeBegin,
                             const int& clusterRangeEnd);
    bool    totInRange      (const int& frameNumber, const int& clusterNumber,
                             const int& totRangeBegin, const int& totRangeEnd);

    QList<ePoint> getListTotInRange(const int& frameNumber, const int& clusterNumber,
                                    const int& totRangeBegin, const int& totRangeEnd) const;

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
