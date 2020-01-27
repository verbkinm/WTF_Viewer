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

    size_t getFrameCount() const;
    size_t getClusterCount(size_t frameNumber) const;
    size_t getClusterLength(size_t frameNumber, size_t clusterNumber) const;

    std::vector<double> getClustersLengthVector() const;
    std::vector<double> getVectorValueTots() const;
    std::vector<double> getVectorSumTots() const;

    size_t getClusterMin() const;
    size_t getClusterMax() const;
    size_t getTotMin() const;
    size_t getTotMax() const;

    OneFrame::cluster getClusterInTotRange(size_t frameNumber, size_t clusterNumber, Range<double> range) const;
    std::map<double, double> getMapOfTotPoints(size_t clusterLenght) const;
    std::map<double, double> getMapOfTotPointsSummarize(size_t clusterLenght) const;
    std::map<double, double> getMapOfClusterSize() const;

    const OneFrame::ePoint& getEPoint(size_t frameNumber, size_t clusterNumber, size_t eventNumber) const;
    OneFrame::ePoint &getPointer_to_EPoint(size_t frameNumber, size_t clusterNumber, size_t eventNumber);

    bool isClusterInRange(size_t clusterLength, Range<size_t> range) const;
    bool isTotInRange (size_t frameNumber, size_t clusterNumber, Range<double> range) const;
    bool isSumTotClusterInRange (size_t frameNumber, size_t clusterNumber, Range<double> range) const;

    void createFromFile(const QString& path);
    void clear();

    enum {ALL_CLUSTER = 0};
    Filter_Clog _filter;

private:
    const std::vector<OneFrame>& getFramesVector() const;
    bool isLineContainsWholeFrame(const QString &line, QStringList &buff);
    void countingTot(size_t frameNumber, size_t clusterNumber, size_t clusterLenght, std::map<double, double> &map) const;
    std::vector<QPointF> getVectorOfPointsFromClusters() const;
    double summarizeTotsInCluster(size_t frameNumber, size_t clusterNumber) const;

    std::vector<OneFrame> _vectorOfFrames;
    size_t _minCluster, _maxCluster,
           _minTot, _maxTot;
};

#endif // FRAMES_H
