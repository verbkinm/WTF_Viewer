#include "settingsimage.h"
#include "ui_settingsimage.h"
#include <QDebug>
#include <QIcon>
#include <QPixmap>
#include <QColorDialog>

SettingsImage::SettingsImage(std::shared_ptr<QSettings> settings, QWidget *parent) :
    QDialog(parent), pSettings(settings),
    ui(new Ui::SettingsImage)
{
    ui->setupUi(this);

    ui->listWidget->setCurrentRow(0);

    readSettings();
}

SettingsImage::~SettingsImage()
{
    delete ui;
}

void SettingsImage::writeSettings()
{
    pSettings.get()->beginGroup("SettingsImage");

    pSettings.get()->setValue("FrameGroupBox", ui->FrameGroupBox->isChecked());
    pSettings.get()->setValue("frameWidth",    ui->frameWidth->value());
    pSettings.get()->setValue("frameValue",    ui->frameValue->value());

    pSettings.get()->setValue("MasquradingGroupBox",   ui->MasquradingGroupBox->isChecked());
    pSettings.get()->setValue("maskBefore",            ui->maskBefore->isChecked());
    pSettings.get()->setValue("maskAfter",             ui->maskAfter->isChecked());
    pSettings.get()->setValue("maskValue",             ui->maskValue->value());
    pSettings.get()->setValue("maskNewValue",          ui->maskNewValue->value());
    pSettings.get()->setValue("maskColor",             ui->maskColor->icon().pixmap(16, 16).toImage().pixelColor(0,0).name());

    pSettings->endGroup();
}

void SettingsImage::on_actionAccepted_triggered()
{
    emit this->accept();
}

void SettingsImage::on_actionReject_triggered()
{
    emit this->reject();
}

void SettingsImage::readSettings()
{
    pSettings->beginGroup("SettingsImage");

    ui->FrameGroupBox->setChecked(pSettings->value("FrameGroupBox", false).toBool());
    ui->frameWidth->setValue(pSettings->value("frameWidth", 0).toInt());
    ui->frameValue->setValue(pSettings->value("frameValue", 0).toInt());

    ui->MasquradingGroupBox->setChecked(pSettings->value("MasquradingGroupBox", false).toBool());
    ui->maskBefore->setChecked(pSettings->value("maskBefore", false).toBool());
    ui->maskAfter->setChecked(pSettings->value("maskAfter", false).toBool());
    ui->maskValue->setValue(pSettings->value("maskValue", 0).toInt());
    ui->maskNewValue->setValue(pSettings->value("maskNewValue", 0).toInt());
    QPixmap pix(16, 16);
    pix.fill(QColor(pSettings->value("maskColor", "#00ff15").toString()));
    ui->maskColor->setIcon(pix);

    pSettings->endGroup();
}

void SettingsImage::on_actionsetColor_triggered()
{
    QPixmap pix(16, 16);

    if(sender()->objectName() == "maskColor")
    {
        QColor color = QColorDialog::getColor(ui->maskColor->icon().pixmap(16, 16).toImage().pixelColor(0,0));
        if(color.spec())
        {
            pix.fill(color);
            ui->maskColor->setIcon(QIcon(pix));
        }
    }
}
