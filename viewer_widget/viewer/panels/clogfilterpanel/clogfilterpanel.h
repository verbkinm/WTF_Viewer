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

    void setClusterRange(const std::pair<float, float> &range);
    void setTotRange(const std::pair<float, float> &range);
    void setTotRangeFull(const std::pair<float, float> &range);

    float getClusterBegin() const;
    float getClusterEnd() const;
    float getTotBegin() const;
    float getTotEnd() const;
    float getTotBeginFull() const;
    float getTotEndFull() const;

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
