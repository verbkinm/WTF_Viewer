#include "viewer_data_panel.h"
#include "ui_viewer_data_panel.h"

_Viewer_Data_Panel::_Viewer_Data_Panel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Viewer_Data_Panel)
{
    ui->setupUi(this);
}

_Viewer_Data_Panel::~_Viewer_Data_Panel()
{
    delete ui;
}

void _Viewer_Data_Panel::setData(int x, int y, double data, size_t width, size_t height)
{
    ui->x->setValue(x);
    ui->y->setValue(y);
    ui->data->setValue(data);
    ui->width->setValue(static_cast<int>(width));
    ui->heigth->setValue(static_cast<int>(height));
}

void _Viewer_Data_Panel::setX(int value)
{
    ui->x->setValue(value);
}

void _Viewer_Data_Panel::setY(int value)
{
    ui->y->setValue(value);
}

void _Viewer_Data_Panel::setData(double value)
{
    ui->data->setValue(value);
}

void _Viewer_Data_Panel::slotSetX(int value)
{
    ui->x->setValue(value);
}

void _Viewer_Data_Panel::slotSetY(int value)
{
    ui->y->setValue(value);
}

void _Viewer_Data_Panel::slotSetData(double value)
{
    ui->data->setValue(value);
}

void _Viewer_Data_Panel::slotSetWidth(int value)
{
    ui->width->setValue(value);
}

void _Viewer_Data_Panel::slotSetHeight(int value)
{
    ui->heigth->setValue(value);
}
