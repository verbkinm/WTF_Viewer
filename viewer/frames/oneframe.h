#ifndef ONEFRAME_H
#define ONEFRAME_H

#include <QList>
#include <QDebug>

struct evPoint
{
    int x = 0;
    int y = 0;
    double tot = 0;

    evPoint(const int& x, const int& y, const double& tot)
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

    void setThreshold_energy        (const double& value);
    void setExposure_time           (const double& value);

    void appendEPoint(const ePoint &point);
    void appendEPoint(const int &x, const int &y, const double &tot);

    void addCluster();
    void addEPoint(cluster& inClaster, const int& x, const int& y, const double &tot);

    int getClusterCount() const;
    int getClusterLenght(const int& clusterNumber) const;
    int getEventCountInCluster(const int& clusterNumber) const;
    const QList<cluster>& getList() const;

    const ePoint &getEPoint(const int& clusterNumber, const int& eventNumber) const;

    void    clear();

private:
    int number              = 0;
    double threshold_energy = 0;
    double exposure_time    = 0; //спросить у Влада за параметры в скобках

    QList<cluster> list;
};

#endif // ONEFRAME_H
