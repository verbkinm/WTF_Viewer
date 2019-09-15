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

    bool isAllTotInCluster() const;

    bool isMediPix() const;
    bool isTotRangeChecked() const;

    void checkedMediPix(bool b);
    void checkedTimePix(bool b);

private:
    Ui::ClogFilterPanel *ui;

signals:
    void signalApplyFilter();

private slots:
    void slotTotType();
protected:
    virtual void keyReleaseEvent(QKeyEvent *event);
};

#endif // CLOGFILTERPANEL_H
