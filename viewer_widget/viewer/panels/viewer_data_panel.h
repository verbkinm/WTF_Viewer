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

    void setData(size_t x, size_t y, double data, size_t width, size_t height);

private:
    Ui::Viewer_Data_Panel *ui;
};

#endif // VIEWER_DATA_PANEL_H
