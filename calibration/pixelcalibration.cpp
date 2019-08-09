#include "pixelcalibration.h"
#include "ui_pixelcalibration.h"

PixelCalibration::PixelCalibration(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PixelCalibration)
{
    ui->setupUi(this);
}

PixelCalibration::~PixelCalibration()
{
    delete ui;
}
