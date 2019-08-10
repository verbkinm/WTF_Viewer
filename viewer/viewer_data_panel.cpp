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

void Viewer_Data_Panel::setAllData(int x, int y, double data, int width, int height)
{
    ui->x->setValue(x);
    ui->y->setValue(y);
    ui->data->setValue(data);
    ui->width->setValue(width);
    ui->heigth->setValue(height);
}

void Viewer_Data_Panel::setX(int value)
{
    ui->x->setValue(value);
}

void Viewer_Data_Panel::setY(int value)
{
    ui->y->setValue(value);
}

void Viewer_Data_Panel::setAllData(double value)
{
    ui->data->setValue(value);
}

void Viewer_Data_Panel::slotSetX(int value)
{
    ui->x->setValue(value);
}

void Viewer_Data_Panel::slotSetY(int value)
{
    ui->y->setValue(value);
}

void Viewer_Data_Panel::slotSetData(double value)
{
    ui->data->setValue(value);
}

void Viewer_Data_Panel::slotSetWidth(int value)
{
    ui->width->setValue(value);
}

void Viewer_Data_Panel::slotSetHeight(int value)
{
    ui->heigth->setValue(value);
}
