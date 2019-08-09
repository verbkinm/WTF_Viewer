#include "settingsimage.h"
#include "ui_settingsimage.h"
#include <QDebug>
#include <QIcon>
#include <QPixmap>
#include <QColorDialog>

SettingsImage::SettingsImage(QSettings &settings, QWidget *parent) :
    QDialog(parent), settings(&settings),
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
    settings->beginGroup("SettingsImage");

    settings->setValue("FrameGroupBox", ui->FrameGroupBox->isChecked());
    settings->setValue("frameWidth",    ui->frameWidth->value());
    settings->setValue("frameValue",    ui->frameValue->value());

    settings->setValue("MasquradingGroupBox",   ui->MasquradingGroupBox->isChecked());
    settings->setValue("maskBefore",            ui->maskBefore->isChecked());
    settings->setValue("maskAfter",             ui->maskAfter->isChecked());
    settings->setValue("maskValue",             ui->maskValue->value());
    settings->setValue("maskNewValue",          ui->maskNewValue->value());
    settings->setValue("maskColor",             ui->maskColor->icon().pixmap(16, 16).toImage().pixelColor(0,0).name());

    settings->endGroup();
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
    settings->beginGroup("SettingsImage");

    ui->FrameGroupBox->setChecked(settings->value("FrameGroupBox", false).toBool());
    ui->frameWidth->setValue(settings->value("frameWidth", 0).toInt());
    ui->frameValue->setValue(settings->value("frameValue", 0).toInt());

    ui->MasquradingGroupBox->setChecked(settings->value("MasquradingGroupBox", false).toBool());
    ui->maskBefore->setChecked(settings->value("maskBefore", false).toBool());
    ui->maskAfter->setChecked(settings->value("maskAfter", false).toBool());
    ui->maskValue->setValue(settings->value("maskValue", 0).toInt());
    ui->maskNewValue->setValue(settings->value("maskNewValue", 0).toInt());
    QPixmap pix(16, 16);
    pix.fill(QColor(settings->value("maskColor", "#00ff15").toString()));
    ui->maskColor->setIcon(pix);

    settings->endGroup();
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
