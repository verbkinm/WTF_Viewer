#include <QFile>
#include <QPointF>
#include <iostream>

#include "frames.h"

Frames::Frames(QObject *parent) : QObject(parent)
{
    
}

size_t Frames::getFrameCount() const
{
    return _vectorOfFrames.size();
}

size_t Frames::getClusterCount(size_t frameNumber) const
{
    try
    {
        return  _vectorOfFrames.at(frameNumber).getClusterCount();
    }
    catch (std::out_of_range&)
    {
        return 0;
    }
}

size_t Frames::getClusterLength(size_t frameNumber, size_t clusterNumber) const
{
    try
    {
        return  _vectorOfFrames.at(frameNumber).getClusterLenght(clusterNumber);
    }
    catch (std::out_of_range&)
    {
        return 0;
    }
}

const OneFrame::ePoint& Frames::getEPoint(size_t frameNumber, size_t clusterNumber, size_t eventNumber) const
{
    try
    {
        return _vectorOfFrames.at(frameNumber).getEPoint(clusterNumber, eventNumber);
    }
    catch (std::out_of_range&)
    {
        return _vectorOfFrames.at(frameNumber).empty_ePoint;
    }
}

OneFrame::ePoint &Frames::getPointer_to_EPoint(size_t frameNumber, size_t clusterNumber, size_t eventNumber)
{
    return _vectorOfFrames[frameNumber].getPointer_to_EPoint(clusterNumber, eventNumber);
}

void Frames::createFromFile(const QString &path)
{
    QFile file(path);
    if(!file.open(QFile::ReadOnly))
    {
        std::cerr << "Can't open CLOG file \"" << path.toStdString() <<"\"";
        return;
    }
    
    QString line;
    QStringList buff;
    while(!file.atEnd())
    {
        line = file.readLine();
        if(isLineContainsWholeFrame(line, buff))
        {
            OneFrame oneFrame;
            oneFrame.createFromStrings(buff);
            _vectorOfFrames.push_back(oneFrame);
            buff.clear();
            buff << line;
        }
    }
    file.close();
    
    OneFrame oneFrame;
    oneFrame.createFromStrings(buff);
    _vectorOfFrames.push_back(oneFrame);
}

void Frames::clear()
{
    _vectorOfFrames.clear();
}

const std::vector<OneFrame> &Frames::getFramesVector() const
{
    return _vectorOfFrames;
}

bool Frames::isClusterInRange(size_t clusterLength, Range<size_t> range) const
{
    if(clusterLength >= range.min() && clusterLength <= range.max())
        return true;
    return false;
}

bool Frames::isTotInRange(size_t frameNumber, size_t clusterNumber, Range<double> range) const
{
    try
    {
        for (auto &point : _vectorOfFrames.at(frameNumber).getClustersVector().at(clusterNumber))
            if(point.tot >= range.min() && point.tot <= range.max())
                return true;
    }
    catch(std::out_of_range&)
    {
        return false;
    }
    return false;
}

bool Frames::isSumTotClusterInRange(size_t frameNumber, size_t clusterNumber, Range<double> range) const
{
    double sum = summarizeTotsInCluster(frameNumber, clusterNumber);
    if(sum >= range.min() && sum <= range.max())
        return true;
    return false;
}

bool Frames::isLineContainsWholeFrame(const QString &line, QStringList &buff)
{
    static bool firstStart = true;
    if(line.startsWith("Frame") && firstStart)
        firstStart = false;
    else if(line.startsWith("Frame") && !firstStart)
        return true;
    
    buff << line;
    return false;
}

OneFrame::cluster Frames::getClusterInTotRange(size_t frameNumber, size_t clusterNumber, Range<double> range) const
{
    std::vector<OneFrame::ePoint> ePointVector;
    try
    {
        for (const auto &point : _vectorOfFrames.at(frameNumber).getClustersVector().at(clusterNumber))
        {
            if(point.tot >= range.min() && point.tot <= range.max())
                ePointVector.push_back(point);
        }
        return ePointVector;
    }
    catch(std::out_of_range&)
    {
        return  ePointVector;
    }
}
std::vector<double> Frames::getClustersLengthVector() const
{
    std::vector<double> lenghtList;
    for (size_t frameNumber = 0; frameNumber < getFrameCount(); ++frameNumber)
        for (size_t clusterNumber = 0; clusterNumber < getClusterCount(frameNumber); ++clusterNumber)
            lenghtList.push_back(getClusterLength(frameNumber, clusterNumber));
    
    std::sort(lenghtList.begin(), lenghtList.end());
    auto last = std::unique(lenghtList.begin(), lenghtList.end());
    lenghtList.erase(last, lenghtList.end());
    
    return lenghtList;
}

std::map<double, double> Frames::getMapOfTotPoints(size_t clusterLenght) const
{
    //key = tot, value = count
    std::map<double, double> map;
    
    for (size_t frameNumber = 0; frameNumber < getFrameCount(); ++frameNumber)
        for (size_t clusterNumber = 0; clusterNumber < getClusterCount(frameNumber); ++clusterNumber)
            countingTot(frameNumber, clusterNumber, clusterLenght, map);

    return map;
}

std::map<double, double> Frames::getMapOfTotPointsSummarize(size_t clusterLenght) const
{
    //key = tot, value = count
    std::map<double, double> map;

    for (size_t frameNumber = 0; frameNumber < getFrameCount(); ++frameNumber)
    {
        for (size_t clusterNumber = 0; clusterNumber < getClusterCount(frameNumber); ++clusterNumber)
        {
            if(getClusterLength(frameNumber, clusterNumber) == clusterLenght || clusterLenght == ALL_CLUSTER)
            {
                double sum = summarizeTotsInCluster(frameNumber, clusterNumber);
                map[sum] = map[sum] + 1;
            }
        }
    }

    return map;
}

std::map<double, double> Frames::getMapOfClusterSize() const
{
    //key = cluster, value = count
    std::map<double, double> map;

    for (size_t frameNumber = 0; frameNumber < getFrameCount(); ++frameNumber)
    {
        for (size_t clusterNumber = 0; clusterNumber < getClusterCount(frameNumber); ++clusterNumber)
        {
            size_t lenght = getClusterLength(frameNumber, clusterNumber);
            map[lenght] = map[lenght] + 1;
        }
    }

    return map;
}

void Frames::countingTot(size_t frameNumber, size_t clusterNumber, size_t clusterLenght, std::map<double, double> &map) const
{
    if(getClusterLength(frameNumber, clusterNumber) == clusterLenght || clusterLenght == ALL_CLUSTER)
    {
        for (size_t eventNumber = 0; eventNumber < getClusterLength(frameNumber, clusterNumber); ++eventNumber)
        {
            double key = getEPoint(frameNumber, clusterNumber, eventNumber).tot;
            map[key] = map[key] + 1;
        }
    }
}

std::vector<QPointF> Frames::getVectorOfPointsFromClusters() const
{
    //key = cluster, value = count
    std::map<size_t, size_t> map;
    
    for (size_t frameNumber = 0; frameNumber < getFrameCount(); ++frameNumber)
        for (size_t clusterNumber = 0; clusterNumber < getClusterCount(frameNumber); ++clusterNumber)
        {
            size_t key = getClusterLength(frameNumber, clusterNumber);
            map[key] = map[key] + 1;
        }
    
    std::vector<QPointF> vector;
    for (auto &[key, value] : map)
        vector.push_back(QPointF(key, value));
    
    return vector;
}

double Frames::summarizeTotsInCluster(size_t frameNumber, size_t clusterNumber) const
{
    double sum = 0;
    
    for (size_t eventNumber = 0; eventNumber < getClusterLength(frameNumber, clusterNumber); ++eventNumber)
        sum += getEPoint(frameNumber, clusterNumber, eventNumber).tot;
    
    return sum;
}

std::vector<double> Frames::getVectorValueTots() const
{
    std::vector<double> totVector;
    for (size_t frameNumber = 0; frameNumber < getFrameCount(); ++frameNumber)
        for (size_t clusterNumber = 0; clusterNumber < getClusterCount(frameNumber); ++clusterNumber)
            for (size_t eventNumber = 0; eventNumber < getClusterLength(frameNumber, clusterNumber); ++eventNumber)
                totVector.push_back(getEPoint(frameNumber, clusterNumber, eventNumber).tot);
    
    std::sort(totVector.begin(), totVector.end());
    auto last = std::unique(totVector.begin(), totVector.end());
    totVector.erase(last, totVector.end());
    
    return totVector;
}

std::vector<double> Frames::getVectorSumTots() const
{
    std::vector<double> sumtVector;

    for (size_t frameNumber = 0; frameNumber < getFrameCount(); ++frameNumber)
        for (size_t clusterNumber = 0; clusterNumber < getClusterCount(frameNumber); ++clusterNumber)
            sumtVector.push_back(summarizeTotsInCluster(frameNumber, clusterNumber));
    
    std::sort(sumtVector.begin(), sumtVector.end());
    auto last = std::unique(sumtVector.begin(), sumtVector.end());
    sumtVector.erase(last, sumtVector.end());
    
    return sumtVector;
}
