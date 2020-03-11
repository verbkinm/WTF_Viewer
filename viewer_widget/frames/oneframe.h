#ifndef ONEFRAME_H
#define ONEFRAME_H

#include <QStringList>
#include <vector>

struct evPoint
{
    unsigned char x;
    unsigned char y;
    float tot;

    evPoint(unsigned char x, unsigned char y, float tot) : x(0), y(0), tot(0)
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

    size_t getNumber() const;
    float getExposure_time() const;
    float getThreshold_energy() const;

    size_t getClusterCount() const;
    size_t getClusterLenght(size_t clusterNumber) const;
    const std::vector<cluster> *getClustersVector() const;

    const ePoint *getEPoint(size_t clusterNumber, size_t eventNumber) const;
    ePoint *getEPoint(size_t clusterNumber, size_t eventNumber);

    std::string toString() const;

    bool setFrameProperties(const QString &string);
    void setClusterProperies(QString &string);

    void appendCluster();

private:
    void appendEPoint(size_t clusterNumber, const ePoint &point);

    void setThreshold_energy (float value);
    void setExposure_time (float value);
    void setFrameNumber(size_t);

    size_t _number;
    float _threshold_energy;
    float _exposure_time;

    std::vector<cluster> _vectorOfCluster;
};

#endif // ONEFRAME_H
