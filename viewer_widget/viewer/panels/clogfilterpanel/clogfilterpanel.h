#ifndef CLOGFILTERPANEL_H
#define CLOGFILTERPANEL_H

#include <QWidget>
#include <QEvent>

#include "../../../frames/frames.h"

namespace Ui {
class ClogFilterPanel;
}

class ClogFilterPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ClogFilterPanel(QWidget *parent = nullptr);
    ~ClogFilterPanel();

    void setClusterRange(const std::vector<double> &vector);
    void setTotRange(const std::vector<double> &vector);
    void setTotRangeFull(const std::vector<double> &vector);

    double getClusterBegin() const;
    double getClusterEnd() const;
    double getTotBegin() const;
    double getTotEnd() const;
    double getTotBeginFull() const;
    double getTotEndFull() const;

    bool isClusterEnable() const;
    bool isTotEnable() const;
    bool isAllTotInCluster() const;

    bool isMediPix() const;
    bool isFullTotRange() const;

    void setAllTotInCluster(bool b);
    void checkedMediPix(bool b);
    void checkedTimePix(bool b);

private:
    Ui::ClogFilterPanel *ui;



//    ClogRanges _clusterRanges, _totRanges, _totFullRanges;
//    QStringList clusterListModel, clusterListBegin, clusterListEnd;
////    QString clusterBeginLast, clusterEndLast;

//    QStringList totListModel, totListBegin, totListEnd;
//    QString totBeginLast, totEndLast;

//    QStringList totListModelFull, totListBeginFull, totListEndFull;
//    QString totBeginLastFull, totEndLastFull;

//    void disconnectSignals() const;
//    void connectSignals() const;

//private slots:
//    void slotEnableRange();
//    void slotSetRange(int currentIndex);

//public slots:
//    void slotApplyFilter();

signals:
    void signalApplyFilter();

protected:
    virtual void keyReleaseEvent(QKeyEvent *event);
};

#endif // CLOGFILTERPANEL_H
