#include "oneframe.h"

OneFrame::OneFrame(int number)
{
    this->number = number;
}

void OneFrame::setThreshold_energy(double value)
{
    threshold_energy = value;
}

void OneFrame::setExposure_time(double value)
{
    exposure_time = value;
}

void OneFrame::appendEPoint(const ePoint& point)
{
    list.last().append(point);
}

void OneFrame::appendEPoint(int x, int y, double tot)
{
    appendEPoint({x, y, tot});
}

void OneFrame::addCluster()
{
    cluster newClaster;
    list.append(newClaster);
}

void OneFrame::addEPoint(cluster& inClaster, int x, int y, double tot)
{
    inClaster.append({x,y,tot});
}
int OneFrame::getClusterCount() const
{
    return int(list.length());
}

int OneFrame::getClusterLenght(int clusterNumber) const
{
    return  int(list.at(int(clusterNumber)).length());
}

int OneFrame::getEventCountInCluster(int clusterNumber) const
{
    if(clusterNumber > list.length() - 1)
    {
        qDebug() << "error in " << Q_FUNC_INFO << __FILE__ << "line: " << __LINE__;
        exit(1);
    }

    return list.at(clusterNumber).length();
}

const QList<cluster> &OneFrame::getList() const
{
    return list;
}

const ePoint& OneFrame::getEPoint(int clusterNumber, int eventNumber) const
{
    if(clusterNumber > list.length() - 1 ||
           eventNumber > list.at(clusterNumber).length())
    {
        qDebug() << "error in " << Q_FUNC_INFO << __FILE__ << "line: " << __LINE__;
        exit(1);
    }

    return list.at(clusterNumber).at(eventNumber);
}

void OneFrame::clear()
{
    foreach (cluster clust, list)
        clust.clear();
}
