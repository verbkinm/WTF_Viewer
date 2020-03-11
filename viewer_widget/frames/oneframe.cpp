#include "oneframe.h"

#include <QDebug>

OneFrame::OneFrame() : _number(0), _threshold_energy(-1), _exposure_time(-1)
{

}

void OneFrame::setThreshold_energy(float value)
{
    _threshold_energy = value;
}
void OneFrame::setExposure_time(float value)
{
    _exposure_time = value;
}

void OneFrame::setFrameNumber(size_t number)
{
    _number = number;
}

void OneFrame::appendEPoint(size_t clusterNumber, const ePoint &point)
{
    try
    {
        _vectorOfCluster.at(clusterNumber).push_back(point);
    }
    catch (std::out_of_range &)
    {
        return;
    }
}

void OneFrame::appendCluster()
{
    _vectorOfCluster.push_back(cluster());
}

size_t OneFrame::getClusterCount() const
{
    return _vectorOfCluster.size();
}

size_t OneFrame::getClusterLenght(size_t clusterNumber) const
{
    try
    {
        return _vectorOfCluster.at(clusterNumber).size();
    }
    catch (std::out_of_range &)
    {
        return 0;
    }
}

const std::vector<OneFrame::cluster> *OneFrame::getClustersVector() const
{
    return &_vectorOfCluster;
}

const OneFrame::ePoint *OneFrame::getEPoint(size_t clusterNumber, size_t eventNumber) const
{
    try
    {
        return &_vectorOfCluster.at(clusterNumber).at(eventNumber);
    }
    catch (std::out_of_range &)
    {
        return nullptr;
    }
}

OneFrame::ePoint *OneFrame::getEPoint(size_t clusterNumber, size_t eventNumber)
{
    try
    {
        return &_vectorOfCluster.at(clusterNumber).at(eventNumber);
    }
    catch (std::out_of_range &)
    {
        return nullptr;
    }
}

std::string OneFrame::toString() const
{
    std::string result;

    result += "Frame " + std::to_string(_number) + " (" + std::to_string(_threshold_energy) + ", " + std::to_string(_exposure_time) + " s)\n";
    for(const auto & cluster : _vectorOfCluster)
    {
        for(const auto & point : cluster)
            result += "[" + std::to_string(point.x) + ", " + std::to_string(point.y) + ", " + std::to_string(point.tot) + "] ";
        result += "\n";
    }

    return result;
}

size_t OneFrame::getNumber() const
{
    return _number;
}

float OneFrame::getThreshold_energy() const
{
    return _threshold_energy;
}

float OneFrame::getExposure_time() const
{
    return _exposure_time;
}

bool OneFrame::setFrameProperties(const QString &string)
{
    QStringList stringSplit = string.split(' ');

    if(stringSplit.length() != 5)
        return false;

    setFrameNumber(stringSplit.at(1).toULongLong());
    QString threshold = stringSplit.at(2);
    threshold.remove(',').remove('(');
    setThreshold_energy(threshold.toFloat());
    setExposure_time(stringSplit.at(3).toFloat());

    return true;
}

void OneFrame::setClusterProperies(QString &string)
{
    string.remove(" ").remove("\r\n").remove(0, 1).chop(1);
    QStringList listFromString = string.split("][");

    for(QString &str : listFromString)
    {
        QStringList point = str.split(",");
        if( (point.length() == 3) && _vectorOfCluster.size())
            appendEPoint(_vectorOfCluster.size() - 1, {static_cast<unsigned char>(point[0].toInt()), static_cast<unsigned char>(point[1].toInt()), point[2].toFloat()});
    }
}
