#ifndef PIX_FILTER_PANEL_H
#define PIX_FILTER_PANEL_H

#include <QWidget>

namespace Ui {
class Pix_Filter_Panel;
}

class Pix_Filter_Panel : public QWidget
{
    Q_OBJECT

public:
    Pix_Filter_Panel(QWidget *parent = nullptr);
    ~Pix_Filter_Panel();

    enum  {PIX_PROPERTY_TAB, CLOG_FILTER_TAB};

    void setX(int);
    void setY(int);
    void setWidth(int);
    void setHeight(int);

    void setDataPanelEnabled(bool);
    void setDataPanelReadOnly(bool);
    void setToggleButtonEnable(bool);
    void setButtonCutDisable(bool);
    void setTabEnable(int, bool);
    void setClusterRange(const std::vector<double> &vector);
    void setTotRange(const std::vector<double> &vector);
    void setTotRangeFull(const std::vector<double> &vector);

    double getClusterBegin() const;
    double getClusterEnd() const;

    double getTotBegin() const;
    double getTotEnd() const;
//    double getTotBeginFull() const;
//    double getTotEndFull() const;

    int getX() const;
    int getY() const;
    int getWidth() const;
    int getHeight() const;
    QColor getPenColor() const;
    double getPenValue() const;

    bool isAllTotInCluster() const;
    bool isMediPix() const;
    bool isTotRangeChecked() const;

    void finishSelection() const;

    void reset(bool);

private:
    Ui::Pix_Filter_Panel *ui;

signals:
    void signalSelectionToggle(bool);
    void signalPenToggle(bool);
    void signalCutClicked(bool);
    void signalRepaint();

    void signalDataOnDataPanelChanged(QRect);
    void signalApplyFilter();

public slots:
    void slotSetDataOnDataPanel(QRect);

private slots:
    void slotDataOnDataPanelChanged();
};

#endif // PIX_FILTER_PANEL_H
