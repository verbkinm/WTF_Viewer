#include "frames.h"
#include "../viewer_widget.h"

#include <QFile>
#include <QApplication>
#include <QFileInfo>
#include <QVector>

#include "iostream"

Frames::Frames(QObject *parent) : QObject(parent)
{

}

void Frames::addFrame(int number)
{
    list.append(OneFrame(number));
}

void Frames::appendCluster()
{
    list.last().addCluster();
}
void Frames::appendEPoint(const ePoint& point)
{
    list.last().appendEPoint(point);
}

void Frames::appendEPoint(int x, int y, double tot)
{
    list.last().appendEPoint(x, y, tot);
}

int Frames::getFrameCount() const
{
    return list.length();
}

int Frames::getClusterCount(int frameNumber) const
{
    if(frameNumber > list.length() - 1)
    {
        std::cerr << "error in " << Q_FUNC_INFO << __FILE__ << "line: " << __LINE__;
        exit(1);
    }

    OneFrame frame = list.at(frameNumber);
    return frame.getClusterCount();
}

int Frames::getClusterLenght(int frameNumber, int clusterNumber) const
{
    OneFrame frame = list.at(frameNumber);
    return frame.getClusterLenght(clusterNumber);
}

int Frames::getEventCountInCluster(int frameNumber, int clusterNumber) const
{
    OneFrame frame = list.at(frameNumber);
    return frame.getEventCountInCluster(clusterNumber);
}

const ePoint& Frames::getEPoint(int frameNumber, int clusterNumber, int eventNumber) const
{
    OneFrame frame = list.at(frameNumber);
    return frame.getEPoint(clusterNumber, eventNumber);
}

void Frames::setFile(const QString &path)
{
    QFile file(path);
    file.open(QFile::ReadOnly);
    QFileInfo fileInfo(path);

    QString     line;
    QStringList lineSplit;
    int         countLine = 0;

    while(!file.atEnd())
    {
        line = file.readLine();
        countLine++;
        QApplication::processEvents();

        if(line.startsWith("Frame"))
        {
            lineSplit = line.split(' ');
            addFrame(lineSplit.at(1).toInt());
//            emit signalFrameCreated(countLine);

        }
        else if(line.startsWith("["))
        {
            //добавляем кластер
            appendCluster();
            line = line.remove(" ").remove("\r\n");
            QStringList lineList;
            foreach (QString str, line.split("]["))
                lineList << str.remove('[').remove(']');

            //заполняем кластер
            int i = 0;
            foreach (QString str, lineList)
            {
                i++;
                QStringList point = str.split(",");
                double tot = point.at(2).toDouble();

                appendEPoint(point.at(0).toInt(),
                             point.at(1).toInt(),
                             tot);
            }
        }
    }
    file.close();

    emit signalFramesCreated();
}
void Frames::clear()
{
    foreach (OneFrame oneFrame, list)
        oneFrame.clear();

    list.clear();
}

const QList<OneFrame> &Frames::getList() const
{
    return list;
}

bool Frames::isClusterInRange(int clusterLength, int clusterRangeBegin, int clusterRangeEnd) const
{
    if(clusterLength >= clusterRangeBegin && clusterLength <= clusterRangeEnd)
        return true;

    return false;
}

bool Frames::isTotInRange(int frameNumber, int clusterNumber, int totRangeBegin, int totRangeEnd) const
{
    OneFrame frame = list.at(frameNumber);
    for (const auto &point : frame.getList().at(clusterNumber))
        if(point.tot >= totRangeBegin && point.tot <= totRangeEnd)
            return true;

    return false;
}

QList<ePoint> Frames::getListTotInRange(int frameNumber, int clusterNumber,
                                        int totRangeBegin, int totRangeEnd) const
{
    OneFrame frame = list.at(frameNumber);
    QList<ePoint> listePoint;
    listePoint.clear();

    for (const auto &point : frame.getList().at(clusterNumber))
        if(point.tot >= totRangeBegin && point.tot <= totRangeEnd)
            listePoint << point;

    return listePoint;
}
QVector<int> Frames::getClustersLenghtList() const
{
    QVector<int> lenghtList;
    for (int frameNumber = 0; frameNumber < getFrameCount(); ++frameNumber)
        for (int clusterNumber = 0; clusterNumber < getClusterCount(frameNumber); ++clusterNumber)
        {
            int lenght = getClusterLenght(frameNumber, clusterNumber);
            if(!lenghtList.contains(lenght))
                lenghtList << lenght;
        }

    std::sort(lenghtList.begin(), lenghtList.end());

    return lenghtList;
}

QVector<QPointF> Frames::getClusterVectorTot(const int &clusterLenght) const
{
    QVector<QPointF> vector;
    //key = tot, value = count
    QMap<double, double> map;

    for (int frameNumber = 0; frameNumber < getFrameCount(); ++frameNumber)
        for (int clusterNumber = 0; clusterNumber < getClusterCount(frameNumber); ++clusterNumber)
            if(getClusterLenght(frameNumber, clusterNumber) == clusterLenght || clusterLenght == 0)
                for (int eventNumber = 0; eventNumber < getEventCountInCluster(frameNumber, clusterNumber); ++eventNumber)
                {
                    double key = getEPoint(frameNumber, clusterNumber, eventNumber).tot;
                    if(map.value(key) == 0.0)
                        map.insert(key, 1);
                    else
                        map[key] = map.value(key) + 1;

                }
    QMapIterator<double, double> i(map);

    while (i.hasNext())
    {
        i.next();
        vector.append(QPointF(i.key(), i.value()));
    }

    return vector;
}

QVector<QPointF> Frames::getClusterVector() const
{
    QVector<QPointF> vector;
    //key = cluster, value = count
    QMap<int, int> map;

    for (int frameNumber = 0; frameNumber < getFrameCount(); ++frameNumber)
        for (int clusterNumber = 0; clusterNumber < getClusterCount(frameNumber); ++clusterNumber)
        {
            int key = getClusterLenght(frameNumber, clusterNumber);
            if(map.value(key) == 0)
                map.insert(key, 1);
            else
                map[key] = map.value(key) + 1;
        }

    QMapIterator<int, int> i(map);

    while (i.hasNext())
    {
        i.next();
        vector.append(QPointF(i.key(), i.value()));
    }

    return vector;
}

QVector<double> Frames::getTotLenghtList() const
{
    QVector<double> lenghtList;
    for (int frameNumber = 0; frameNumber < getFrameCount(); ++frameNumber)
        for (int clusterNumber = 0; clusterNumber < getClusterCount(frameNumber); ++clusterNumber)
            for (int eventNumber = 0; eventNumber < getEventCountInCluster(frameNumber, clusterNumber); ++eventNumber) {
                double tot = getEPoint(frameNumber, clusterNumber, eventNumber).tot;
                if(!lenghtList.contains(tot))
                    lenghtList << tot;
            }

    std::sort(lenghtList.begin(), lenghtList.end());

    return lenghtList;
}
