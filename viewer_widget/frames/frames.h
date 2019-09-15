#ifndef FRAMES_H
#define FRAMES_H

#include <QObject>

#include "oneframe.h"


class Frames : public QObject
{
    Q_OBJECT

public:
    Frames(QObject *parent = nullptr);

    size_t getFrameCount() const;
    size_t getClusterCount(size_t frameNumber) const;
    size_t getClusterLength(size_t frameNumber, size_t clusterNumber) const;
    std::vector<double> getClustersLengthVector() const;
    std::vector<double> getVectoValueTots() const;
    std::vector<double> getVectorSumTots() const;
    std::vector<double> getVectorSumValueTots() const;
    OneFrame::cluster getClusterTotInRange(size_t frameNumber, size_t clusterNumber, double totRangeBegin, double totRangeEnd) const;
    std::map<double, double> getVectorOfPointsFromTots(size_t clusterLenght) const;

    const OneFrame::ePoint& getEPoint(size_t frameNumber, size_t clusterNumber, size_t eventNumber) const;

    bool isClusterInRange(size_t clusterLength, size_t clusterRangeBegin, size_t clusterRangeEnd) const;
    bool isTotInRange (size_t frameNumber, size_t clusterNumber, double totRangeBegin, double totRangeEnd) const;
    bool isSumTotClusterInRange (size_t frameNumber, size_t clusterNumber, double totRangeBeginFull, double totRangeEndFull) const;

    void createFromFile(const QString& path);
    void clear();

    enum {ALL_CLUSTER = 0};

private:
    const std::vector<OneFrame>& getFramesVector() const;
    bool isLineContainsWholeFrame(const QString &line, QStringList &buff);
    void generalCalibrationSettingsForArray(OneFrame::ePoint &point);
    void countingTot(size_t frameNumber, size_t clusterNumber, size_t clusterLenght, std::map<double, double> &map) const;
    std::vector<QPointF> getVectorOfPointsFromClusters() const;
    double summarizeTotsInCluster(size_t frameNumber, size_t clusterNumber) const;


    std::vector<OneFrame> _vectorOfFrames;

signals:
    void signalFramesCreated();
};

#endif // FRAMES_H
