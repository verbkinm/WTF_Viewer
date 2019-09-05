#ifndef CLOGFILTERPANEL_H
#define CLOGFILTERPANEL_H

#include <QWidget>
#include <QEvent>

#include "../../frames/frames.h"

namespace Ui {
class ClogFilterPanel;
}

class ClogFilterPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ClogFilterPanel(QWidget *parent = nullptr);
    ~ClogFilterPanel();

    void setClusterRange(const std::vector<size_t> &vector);
    void setTotRange(const std::vector<double> &vector);
    void setTotRangeFull(const std::vector<double> &vector);

    size_t getClusterBegin() const;
    size_t getClusterEnd() const;

    size_t getTotBegin() const;
    size_t getTotEnd() const;

    bool isClusterEnable() const;
    bool isTotEnable() const;
    bool isAllTotInCluster() const;

    bool isMediPix() const;

    void setClusterBegin(int v);
    void setClusterEnd(int v);

    void setTotBegin(int v);
    void setTotEnd(int v);

    void setCluster(bool b);
    void setTot(bool b);
    void setAllTotInCluster(bool b);
    void setMediPix(bool b);
    void setTimePix(bool b);

private:
    Ui::ClogFilterPanel *ui;

    QStringList clusterListModel, clusterListBegin, clusterListEnd;
    QString clusterBeginLast, clusterEndLast;

    QStringList totListModel, totListBegin, totListEnd;
    QString totBeginLast, totEndLast;

    QStringList totListModelFull, totListBeginFull, totListEndFull;
    QString totBeginLastFull, totEndLastFull;

    void disconnectSignals() const;
    void connectSignals() const;

private slots:
    void slotEnableRange();
    void slotDates(QString value);

public slots:
    void slotApplyFilter();

signals:
    void signalApplyFilter();

    void signalPixGroupMidiPixSet(bool);

protected:
    virtual void keyReleaseEvent(QKeyEvent *event);
    bool event(QEvent *event);
};

#endif // CLOGFILTERPANEL_H
