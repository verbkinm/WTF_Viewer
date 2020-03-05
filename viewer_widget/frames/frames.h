#ifndef FRAMES_H
#define FRAMES_H

#include <QObject>

#include "oneframe.h"
#include "../viewer/viewer_processor/filter_clog.h"
#include "../../graph/range.h"

class Frames : public QObject
{
    Q_OBJECT

public:
    Frames(QObject *parent = nullptr);

    const OneFrame *getOneFrame(size_t number_of_frame) const;
    size_t getFrameCount() const;
    size_t getClusterCount(size_t frameNumber) const;
    size_t getClusterLength(size_t frameNumber, size_t clusterNumber) const;

    std::vector<size_t> getClustersLengthVector() const;
    std::vector<float> getVectorValueTots() const;
    std::vector<float> getVectorSumTots() const;

    size_t getFrameMin() const;
    size_t getFrameMax() const;

    size_t getClusterMin() const;
    size_t getClusterMax() const;

    float getTotMin() const;
    float getTotMax() const;

    float getSumTotMin() const;
    float getSumTotMax() const;

    float getExposure_time(size_t frameNumber) const;

    OneFrame::cluster getClusterInTotRange(size_t frameNumber, size_t clusterNumber, Range<float> range) const;
    std::map<float, float> getMapOfTotPoints(size_t clusterLenght) const;
    std::map<float, float> getMapOfTotPointsSummarize(size_t clusterLenght) const;
    std::map<float, float> getMapOfClusterSize() const;

    const OneFrame::ePoint *getEPoint(size_t frameNumber, size_t clusterNumber, size_t eventNumber) const;
    OneFrame::ePoint *getEPoint(size_t frameNumber, size_t clusterNumber, size_t eventNumber);

    bool isClusterInRange(size_t clusterLength, Range<size_t> range) const;
    bool isTotInRange (size_t frameNumber, size_t clusterNumber, Range<float> range) const;
    bool isSumTotClusterInRange (size_t frameNumber, size_t clusterNumber, Range<float> range) const;

    void createFromFile(const QString& path);
    void clear();

    enum {ALL_CLUSTER = 0};
    Filter_Clog _filter;

private:
    bool isLineContainsWholeFrame(const QString &line, QStringList &buff, bool &firstStart);
    void countingTot(size_t frameNumber, size_t clusterNumber, size_t clusterLenght, std::map<float, float> &map) const;
    std::vector<QPointF> getVectorOfPointsFromClusters() const;
    float summarizeTotsInCluster(size_t frameNumber, size_t clusterNumber) const;
    void setRanges();
    void setRangeClusters(size_t frameNumber, size_t clusterNumber);
    void setRangeTots(size_t frameNumber, size_t clusterNumber, size_t eventNumber);
    void setRangeSumTots(size_t frameNumber, size_t clusterNumber);
    OneFrame *lastFrame();

    std::vector<OneFrame> _vectorOfFrames;
    size_t _minCluster, _maxCluster;
    float _minTot, _maxTot, _minSumTot, _maxSumTot;
};

#endif // FRAMES_H
