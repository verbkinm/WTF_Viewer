#ifndef CLOGFILTERPANEL_H
#define CLOGFILTERPANEL_H

#include <QWidget>
#include <QEvent>

#include "frames/frames.h"

namespace Ui {
class ClogFilterPanel;
}

class ClogFilterPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ClogFilterPanel(QWidget *parent = nullptr);
    ~ClogFilterPanel();

    void    setClusterRange(QVector<int> vector);
    void    setTotRange(QVector<double> vector);

    int     getClusterBegin() const;
    int     getClusterEnd() const;

    int     getTotBegin() const;
    int     getTotEnd() const;

    bool    isClusterEnable();
    bool    isTotEnable();
    bool    isAllTotInCluster();

    bool    isMediPix();

    void    setClusterBegin(int v);
    void    setClusterEnd(int v);

    void    setTotBegin(int v);
    void    setTotEnd(int v);

    void    setCluster(bool b);
    void    setTot(bool b);
    void    setAllTotInCluster(bool b);
    void    setMediPix(bool b);
    void    setTimePix(bool b);

private:
    Ui::ClogFilterPanel *ui;

    QStringList clusterListModel, clusterListBegin, clusterListEnd;
    QString clusterBeginLast, clusterEndLast;

    QStringList totListModel, totListBegin, totListEnd;
    QString totBeginLast, totEndLast;

    void    disconnectSignals       ();
    void    connectSignals          ();

private slots:
    void    slotEnableRange         ();

    void    slotDates               (QString value);

public slots:
    void    slotApplyFilter         ();

signals:
    void    signalApplyFilter       ();

    void    signalPixGroupMidiPixSet(bool);

protected:

    virtual void keyReleaseEvent(QKeyEvent *event);
    bool event(QEvent *event);
};

#endif // CLOGFILTERPANEL_H
