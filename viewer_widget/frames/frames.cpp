#include <QFile>
#include <QPointF>
#include <iostream>
#include <limits>

#include "frames.h"

Frames::Frames(QObject *parent) : QObject(parent),
    _minCluster(std::numeric_limits<size_t>::max()), _maxCluster(std::numeric_limits<size_t>::min()),
    _minTot(std::numeric_limits<float>::max()), _maxTot(std::numeric_limits<float>::min()),
    _minSumTot(std::numeric_limits<float>::max()), _maxSumTot(std::numeric_limits<float>::min())
{

}

const OneFrame *Frames::getOneFrame(size_t number_of_frame) const
{
    try
    {
        return  &_vectorOfFrames.at(number_of_frame);
    }
    catch (std::out_of_range &)
    {
        return nullptr;
    }
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
    catch (std::out_of_range &)
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
    catch (std::out_of_range &)
    {
        return 0;
    }
}

const OneFrame::ePoint *Frames::getEPoint(size_t frameNumber, size_t clusterNumber, size_t eventNumber) const
{
    try
    {
        return _vectorOfFrames.at(frameNumber).getEPoint(clusterNumber, eventNumber);
    }
    catch (std::out_of_range &)
    {
        return nullptr;
    }
}

OneFrame::ePoint *Frames::getEPoint(size_t frameNumber, size_t clusterNumber, size_t eventNumber)
{
    try
    {
        return _vectorOfFrames.at(frameNumber).getEPoint(clusterNumber, eventNumber);
    }
    catch (std::out_of_range &)
    {
        return nullptr;
    }
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
    while(!file.atEnd())
    {
        line = file.readLine();
        if(line[0] == 'F')
        {
            OneFrame oneFrame;
            if(oneFrame.setFrameProperties(line))
                _vectorOfFrames.push_back(oneFrame);
            else
                std::cerr << "error file syntax in string \"" << line.toStdString() << "\"\n";
        }
        else if (line[0] == '[')
        {
            auto frame = lastFrame();
            if(frame != nullptr)
            {
                frame->appendCluster();
                frame->setClusterProperies(line);
            }
            else
                std::cerr << "error file syntax in string \"" << line.toStdString() << "\"\n";
        }
    }
    file.close();
    setRanges();
}

void Frames::clear()
{
    _vectorOfFrames.clear();
}

bool Frames::isClusterInRange(size_t clusterLength, Range<size_t> range) const
{
    if(clusterLength >= range.min() && clusterLength <= range.max())
        return true;
    return false;
}

bool Frames::isTotInRange(size_t frameNumber, size_t clusterNumber, Range<float> range) const
{
    try
    {
        for (auto &point : _vectorOfFrames.at(frameNumber).getClustersVector()->at(clusterNumber))
            if(point.tot >= range.min() && point.tot <= range.max())
                return true;
    }
    catch(std::out_of_range &)
    {
        return false;
    }
    return false;
}

bool Frames::isSumTotClusterInRange(size_t frameNumber, size_t clusterNumber, Range<float> range) const
{
    float sum = summarizeTotsInCluster(frameNumber, clusterNumber);
    if(sum >= range.min() && sum <= range.max())
        return true;
    return false;
}

bool Frames::isLineContainsWholeFrame(const QString &line, QStringList &buff, bool &firstStart)
{
    if( (line[0] == 'F') && firstStart)
        firstStart = false;
    else if( (line[0] == 'F') && !firstStart)
        return true;

    buff << line;
    return false;
}

OneFrame::cluster Frames::getClusterInTotRange(size_t frameNumber, size_t clusterNumber, Range<float> range) const
{
    OneFrame::cluster ePointVector;
    try
    {
        for(const auto &point : _vectorOfFrames.at(frameNumber).getClustersVector()->at(clusterNumber))
        {
            if(point.tot >= range.min() && point.tot <= range.max())
                ePointVector.push_back(point);
        }
        return ePointVector;
    }
    catch(std::out_of_range &)
    {
        return  ePointVector;
    }
}
std::vector<size_t> Frames::getClustersLengthVector() const
{
    std::vector<size_t> lenghtList;
    for (size_t frameNumber = _filter._frameBegin; frameNumber <= _filter._frameEnd; ++frameNumber)
        for (size_t clusterNumber = 0; clusterNumber < getClusterCount(frameNumber); ++clusterNumber)
            lenghtList.push_back(getClusterLength(frameNumber, clusterNumber));

    std::sort(lenghtList.begin(), lenghtList.end());
    auto last = std::unique(lenghtList.begin(), lenghtList.end());
    lenghtList.erase(last, lenghtList.end());

    return lenghtList;
}

std::map<float, float> Frames::getMapOfTotPoints(size_t clusterLenght) const
{
    //key = tot, value = count
    std::map<float, float> map;

    for (size_t frameNumber = _filter._frameBegin; frameNumber <= _filter._frameEnd; ++frameNumber)
        for (size_t clusterNumber = 0; clusterNumber < getClusterCount(frameNumber); ++clusterNumber)
            countingTot(frameNumber, clusterNumber, clusterLenght, map);

    return map;
}

std::map<float, float> Frames::getMapOfTotPointsSummarize(size_t clusterLenght) const
{
    //key = tot, value = count
    std::map<float, float> map;

    for (size_t frameNumber = _filter._frameBegin; frameNumber <= _filter._frameBegin; ++frameNumber)
        for (size_t clusterNumber = 0; clusterNumber < getClusterCount(frameNumber); ++clusterNumber)
            if( (getClusterLength(frameNumber, clusterNumber) == clusterLenght) || (clusterLenght == ALL_CLUSTER))
            {
                float sum = summarizeTotsInCluster(frameNumber, clusterNumber);
                map[sum] = map[sum] + 1;
            }

    return map;
}

std::map<float, float> Frames::getMapOfClusterSize() const
{
    //key = cluster, value = count
    std::map<float, float> map;

    for (size_t frameNumber = _filter._frameBegin; frameNumber <= _filter._frameEnd; ++frameNumber)
        for (size_t clusterNumber = 0; clusterNumber < getClusterCount(frameNumber); ++clusterNumber)
        {
            float lenght = getClusterLength(frameNumber, clusterNumber);
            map[lenght] = map[lenght] + 1;
        }

    return map;
}

void Frames::countingTot(size_t frameNumber, size_t clusterNumber, size_t clusterLenght, std::map<float, float> &map) const
{
    size_t cl = getClusterLength(frameNumber, clusterNumber);
    if(  (cl != clusterLenght) || (clusterLenght != ALL_CLUSTER))
        return;

    for (size_t eventNumber = 0; eventNumber < cl; ++eventNumber)
    {
        auto res = getEPoint(frameNumber, clusterNumber, eventNumber);
        if(res)
        {
            float key = res->tot;
            map[key] = map[key] + 1;
        }
    }
}

std::vector<QPointF> Frames::getVectorOfPointsFromClusters() const
{
    //key = cluster, value = count
    std::map<size_t, size_t> map;

    for (size_t frameNumber = _filter._frameBegin; frameNumber <= _filter._frameEnd; ++frameNumber)
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

float Frames::summarizeTotsInCluster(size_t frameNumber, size_t clusterNumber) const
{
    float sum = 0;
    for (size_t eventNumber = 0; eventNumber < getClusterLength(frameNumber, clusterNumber); ++eventNumber)
    {
        auto res = getEPoint(frameNumber, clusterNumber, eventNumber);
        if(res)
            sum += res->tot;
    }

    return sum;
}

void Frames::setRangeClusters(size_t frameNumber, size_t clusterNumber)
{
    size_t lenght = getClusterLength(frameNumber, clusterNumber);
    if(lenght > _maxCluster)
        _maxCluster = lenght;
    else if(lenght < _minCluster)
        _minCluster = lenght;
}

void Frames::setRangeTots(size_t frameNumber, size_t clusterNumber, size_t eventNumber)
{
    auto ePoint = getEPoint(frameNumber, clusterNumber, eventNumber);
    if(!ePoint)
        return;

    if(ePoint->tot > _maxTot)
        _maxTot = ePoint->tot;
    else if(ePoint->tot < _minTot)
        _minTot = ePoint->tot;
}

void Frames::setRangeSumTots(size_t frameNumber, size_t clusterNumber)
{
    float sum = summarizeTotsInCluster(frameNumber, clusterNumber);
    if(sum > _maxSumTot)
        _maxSumTot = sum;
    else if(sum < _minSumTot)
        _minSumTot = sum;
}

void Frames::setRanges()
{
    for (size_t frameNumber = 0; frameNumber < getFrameCount(); ++frameNumber)
        for (size_t clusterNumber = 0; clusterNumber < getClusterCount(frameNumber); ++clusterNumber)
        {
            setRangeSumTots(frameNumber, clusterNumber);
            setRangeClusters(frameNumber, clusterNumber);
            for (size_t eventNumber = 0; eventNumber < getClusterLength(frameNumber, clusterNumber); ++eventNumber)
                setRangeTots(frameNumber, clusterNumber, eventNumber);
        }
}

OneFrame *Frames::lastFrame()
{
    if(_vectorOfFrames.size())
        return &_vectorOfFrames[_vectorOfFrames.size() - 1];
    else
        return nullptr;
}

std::vector<float> Frames::getVectorValueTots() const
{
    std::vector<float> totVector;
    for (size_t frameNumber = _filter._frameBegin; frameNumber <= _filter._frameEnd; ++frameNumber)
        for (size_t clusterNumber = 0; clusterNumber < getClusterCount(frameNumber); ++clusterNumber)
            for (size_t eventNumber = 0; eventNumber < getClusterLength(frameNumber, clusterNumber); ++eventNumber)
            {
                auto res = getEPoint(frameNumber, clusterNumber, eventNumber);
                if(res)
                    totVector.push_back(res->tot);
            }

    std::sort(totVector.begin(), totVector.end());
    auto last = std::unique(totVector.begin(), totVector.end());
    totVector.erase(last, totVector.end());

    return totVector;
}

std::vector<float> Frames::getVectorSumTots() const
{
    std::vector<float> sumtVector;

    for (size_t frameNumber = _filter._frameBegin; frameNumber <= _filter._frameEnd; ++frameNumber)
        for (size_t clusterNumber = 0; clusterNumber < getClusterCount(frameNumber); ++clusterNumber)
            sumtVector.push_back(summarizeTotsInCluster(frameNumber, clusterNumber));

    std::sort(sumtVector.begin(), sumtVector.end());
    auto last = std::unique(sumtVector.begin(), sumtVector.end());
    sumtVector.erase(last, sumtVector.end());

    return sumtVector;
}

size_t Frames::getFrameMin() const
{
    if(_vectorOfFrames.size())
        return _vectorOfFrames.at(0).getNumber();
    return 0;
}

size_t Frames::getFrameMax() const
{
    size_t vec_size = _vectorOfFrames.size();
    if(vec_size)
        return _vectorOfFrames.at(_vectorOfFrames.size() - 1).getNumber();
    return 0;
}

size_t Frames::getClusterMin() const
{
    return _minCluster;
}

size_t Frames::getClusterMax() const
{
    return _maxCluster;
}

float Frames::getTotMin() const
{
    return _minTot;
}

float Frames::getTotMax() const
{
    return _maxTot;
}

float Frames::getSumTotMin() const
{
    return _minSumTot;
}

float Frames::getSumTotMax() const
{
    return _maxSumTot;
}

float Frames::getExposure_time(size_t frameNumber) const
{
    try
    {
        return _vectorOfFrames.at(frameNumber).getExposure_time();
    }
    catch(std::out_of_range &)
    {
        return  -1;
    }
}
