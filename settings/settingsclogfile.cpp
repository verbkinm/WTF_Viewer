#include "settingsclogfile.h"
#include "ui_settingsclogfile.h"

SettingsClogFile::SettingsClogFile(std::shared_ptr<QSettings> settings, QWidget *parent) :
    QDialog(parent), pSettings(settings),
    ui(new Ui::SettingsClogFile)
{
    ui->setupUi(this);

    readSettings();
}

SettingsClogFile::~SettingsClogFile()
{
    delete ui;
}

void SettingsClogFile::writeSettings()
{
    pSettings->beginGroup("SettingsClogFile");

    pSettings->setValue("totMode", ui->totMode->isChecked());
    pSettings->setValue("pixelCalibration", ui->pixelCalibration->isChecked());
    pSettings->setValue("generalCalibration", ui->generalCalibration->isChecked());

    pSettings->endGroup();
}

void SettingsClogFile::readSettings()
{
    pSettings->beginGroup("SettingsClogFile");

    ui->totMode->setChecked(pSettings->value("totMode", true).toBool());
    ui->pixelCalibration->setChecked(pSettings->value("pixelCalibration", false).toBool());
    ui->generalCalibration->setChecked(pSettings->value("generalCalibration", false).toBool());

    pSettings->endGroup();
}
