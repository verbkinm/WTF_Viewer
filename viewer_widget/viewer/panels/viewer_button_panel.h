#ifndef VIEWER_BUTTON_PANEL_H
#define VIEWER_BUTTON_PANEL_H

#include <QWidget>

namespace Ui {
class Viewer_Button_Panel;
}

class Viewer_Button_Panel : public QWidget
{
    Q_OBJECT

public:
    explicit Viewer_Button_Panel(QWidget *parent = nullptr);
    ~Viewer_Button_Panel();

    double getAngle();

private:
    Ui::Viewer_Button_Panel *ui;

signals:
    void signalSaveToBmp();
    void signalSaveToTxt();
    void signalMirrorHorizontal();
    void signalMirrorVertical();
    void signalRotatePlus();
    void signalRotateMinus();
    void signalScaledPlus();
    void signalScaledMinus();
    void signalAngle(double);
    void signalResetTransform();

};

#endif // VIEWER_BUTTON_PANEL_H
