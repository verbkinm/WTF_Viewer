#ifndef ONEFRAME_H
#define ONEFRAME_H

#include <QStringList>
#include <vector>

struct evPoint
{
    size_t x;
    size_t y;
    double tot;

    evPoint(size_t x, size_t y, double tot) : x(0), y(0), tot(0)
    {
        this->x = x;
        this->y = y;
        this->tot = tot;
    }
};

class OneFrame
{
public:

    OneFrame();

    typedef struct evPoint ePoint;
    typedef std::vector<ePoint> cluster;

    bool createFromStrings(QStringList buff);

    size_t getClusterCount() const;
    size_t getClusterLenght(size_t clusterNumber) const;
    size_t getEventCountInCluster(size_t clusterNumber) const;
    const std::vector<cluster> &getClustersVector() const;

    const ePoint &getEPoint(size_t clusterNumber, size_t eventNumber) const;
    ePoint &getPointer_to_EPoint(size_t clusterNumber, size_t eventNumber);

    const ePoint empty_ePoint;

private:
    int _number;
    double _threshold_energy;
    double _exposure_time;

    std::vector<cluster> _vectorOfCluster;

    void appendCluster();
    void appendEPoint(const size_t &clusterNumber, const ePoint &point);

    void setFrameProperties(QString &string);
    void setClusterProperies(QString &string);

    void setThreshold_energy (double value) noexcept;
    void setExposure_time (double value) noexcept;
    void setFrameNumber(int) noexcept;
};

#endif // ONEFRAME_H
