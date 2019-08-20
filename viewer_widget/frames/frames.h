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
    std::vector<size_t> getClustersLengthVector() const;
    std::vector<double> getVectoValueTots() const;
    OneFrame::cluster getClusterTotInRange(size_t frameNumber, size_t clusterNumber,
                                            size_t totRangeBegin, size_t totRangeEnd) const;
    const OneFrame::ePoint& getEPoint(size_t frameNumber, size_t clusterNumber, size_t eventNumber) const;

    bool isClusterInRange(size_t clusterLength, size_t clusterRangeBegin, size_t clusterRangeEnd) const;
    bool isTotInRange (size_t frameNumber, size_t clusterNumber, size_t totRangeBegin, size_t totRangeEnd) const;

    void createFromFile(const QString& path);
    void clear();

private:
    const std::vector<OneFrame>& getFramesVector() const;
    bool isLineContainsWholeFrame(const QString &line, QStringList &buff);
    void generalCalibrationSettingsForArray(OneFrame::ePoint &point);
    std::vector<QPointF> vectorOfPointsFromTots(size_t clusterLenght) const;
    void countingTot(size_t frameNumber, size_t clusterNumber, size_t clusterLenght, std::map<double, double> &map) const;
    std::vector<QPointF> getVectorOfPointsFromClusters() const;

private:
    std::vector<OneFrame> _vectorOfFrames;

signals:
    void signalFramesCreated();
};

#endif // FRAMES_H
