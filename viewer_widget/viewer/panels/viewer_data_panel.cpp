#include "viewer_data_panel.h"
#include "ui_viewer_data_panel.h"

Viewer_Data_Panel::Viewer_Data_Panel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Viewer_Data_Panel)
{
    ui->setupUi(this);
}

Viewer_Data_Panel::~Viewer_Data_Panel()
{
    delete ui;
}

void Viewer_Data_Panel::setData(size_t x, size_t y, double data, size_t width, size_t height)
{
    if(!this->isEnabled())
        return;
    if(static_cast<size_t>(x) > width || static_cast<size_t>(y) > height)
    {
        ui->x->setValue(0);
        ui->y->setValue(0);
        ui->data->setValue(0);
        return;
    }
    ui->x->setValue(static_cast<int>(x));
    ui->y->setValue(static_cast<int>(y));
    ui->data->setValue(data);
    ui->width->setValue(static_cast<int>(width));
    ui->heigth->setValue(static_cast<int>(height));
}
