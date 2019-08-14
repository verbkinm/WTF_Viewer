#ifndef ONEFRAME_H
#define ONEFRAME_H

#include <QList>
#include <QDebug>

struct evPoint
{
    int x = 0;
    int y = 0;
    double tot = 0;

    evPoint(int x, int y, double tot)
    {
        this->x = x;
        this->y = y;
        this->tot = tot;
    }
};

typedef struct evPoint ePoint;
typedef QList<ePoint> cluster;

class OneFrame
{
public:
    OneFrame(int number);

    void setThreshold_energy        (double value);
    void setExposure_time           (double value);

    void appendEPoint(const ePoint &point);
    void appendEPoint(int x, int y, double tot);

    void addCluster();
    void addEPoint(cluster& inClaster, int x, int y, double tot);

    int getClusterCount() const;
    int getClusterLenght(int clusterNumber) const;
    int getEventCountInCluster(int clusterNumber) const;
    const QList<cluster>& getList() const;

    const ePoint &getEPoint(int clusterNumber, int eventNumber) const;

    void clear();

private:
    int number              = 0;
    double threshold_energy = 0;
    double exposure_time    = 0; //спросить у Влада за параметры в скобках

    QList<cluster> list;
};

#endif // ONEFRAME_H
