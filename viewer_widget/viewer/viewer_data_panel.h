#ifndef VIEWER_DATA_PANEL_H
#define VIEWER_DATA_PANEL_H

#include <QWidget>

namespace Ui {
class Viewer_Data_Panel;
}

class Viewer_Data_Panel : public QWidget
{
    Q_OBJECT

public:
    explicit Viewer_Data_Panel(QWidget *parent = nullptr);
    ~Viewer_Data_Panel();

    void setData(int x, int y, double data, int width, int height);
    void setX(int);
    void setY(int);
    void setData(double);

public slots:
    void slotSetX(int);
    void slotSetY(int);
    void slotSetData(double);
    void slotSetWidth(int);
    void slotSetHeight(int);

private:
    Ui::Viewer_Data_Panel *ui;
};

#endif // VIEWER_DATA_PANEL_H
