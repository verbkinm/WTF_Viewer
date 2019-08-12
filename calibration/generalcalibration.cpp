#include "generalcalibration.h"
#include "ui_generalcalibration.h"

#include <QDebug>

GeneralCalibration::GeneralCalibration(QSettings& settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GeneralCalibration), pSettings(&settings)
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
    pSettings->beginGroup("GeneralCalibration");

    pSettings->setValue("A", ui->A->value());
    pSettings->setValue("B", ui->B->value());
    pSettings->setValue("C", ui->C->value());
    pSettings->setValue("T", ui->T->value());
    pSettings->setValue("apply", ui->coefficients->isChecked());

    pSettings->endGroup();
}

void GeneralCalibration::readSettings()
{
    pSettings->beginGroup("GeneralCalibration");

    ui->A->setValue(pSettings->value("A", 0.0).toDouble());
    ui->B->setValue(pSettings->value("B", 0.0).toDouble());
    ui->C->setValue(pSettings->value("C", 0.0).toDouble());
    ui->T->setValue(pSettings->value("T", 0.0).toDouble());
    ui->coefficients->setChecked(pSettings->value("apply", false).toBool());

    pSettings->endGroup();
}
