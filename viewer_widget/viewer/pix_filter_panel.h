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
    Pix_Filter_Panel (const Pix_Filter_Panel &object);
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
    void setClusterRange(const std::vector<size_t> &);
    void setTotRange(const std::vector<double> &);

    size_t getClusterBegin() const;
    size_t getClusterEnd() const;
    size_t getTotBegin() const;
    size_t getTotEnd() const;
    int getX() const;
    int getY() const;
    int getWidth() const;
    int getHeight() const;
    QColor getPenColor() const;
    double getPenValue() const;

    bool isAllTotInCluster() const;
    bool isMediPix() const;

    void finishSelection() const;

    void reset(bool);

private:
    Ui::Pix_Filter_Panel *ui;

signals:
    void signalSelectionToggle(bool);
    void signalPenToggle(bool);
    void signalCutClicked(bool);
    void signalRepaint();

    void signalValueX_Changed(int);
    void signalValueY_Changed(int);
    void signalValueWidth_Changed(int);
    void signalValueHeight_Changed(int);
    void signalApplyFilter();
};

#endif // PIX_FILTER_PANEL_H
