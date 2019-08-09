#include "generalcalibration.h"
#include "ui_generalcalibration.h"

GeneralCalibration::GeneralCalibration(QSettings& settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GeneralCalibration), settings(&settings)
{
    ui->setupUi(this);

    readSettings();
}

GeneralCalibration::~GeneralCalibration()
{
    delete ui;
}

double GeneralCalibration::getA()
{
    return  ui->A->value();
}

double GeneralCalibration::getB()
{
    return  ui->B->value();
}

double GeneralCalibration::getC()
{
    return  ui->C->value();
}

double GeneralCalibration::getT()
{
    return  ui->T->value();
}

bool GeneralCalibration::getApply()
{
    return ui->coefficients->isChecked();
}

void GeneralCalibration::writeSettings()
{
    settings->beginGroup("GeneralCalibration");

    settings->setValue("A", ui->A->value());
    settings->setValue("B", ui->B->value());
    settings->setValue("C", ui->C->value());
    settings->setValue("T", ui->T->value());
    settings->setValue("apply", ui->coefficients->isChecked());

    settings->endGroup();
}

void GeneralCalibration::readSettings()
{
    settings->beginGroup("GeneralCalibration");

    ui->A->setValue(settings->value("A", 0.0).toDouble());
    ui->B->setValue(settings->value("B", 0.0).toDouble());
    ui->C->setValue(settings->value("C", 0.0).toDouble());
    ui->T->setValue(settings->value("T", 0.0).toDouble());
    ui->coefficients->setChecked(settings->value("apply", false).toBool());

    settings->endGroup();
}
