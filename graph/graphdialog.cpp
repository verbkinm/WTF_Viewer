#include "graphdialog.h"
#include "ui_graphdialog.h"

#include <QDebug>

GraphDialog::GraphDialog(std::shared_ptr<QSettings> settings, const Frames &frames, QWidget *parent) :
    QDialog(parent),
    _NEW_WINDOW("New Window"),
    ui(new Ui::GraphDialog),
    pSettings(settings)
{
    ui->setupUi(this);

    ui->DataY->addItem("All ");
    for(auto &value : frames.getClustersLengthVector())
        ui->DataY->addItem(QString::number(value));

    connect(ui->DataX, &QComboBox::currentTextChanged, this, &GraphDialog::slotSelectDataX);

    ui->windowGraph->addItem(_NEW_WINDOW);

    pSettings->beginGroup("GeneralCalibration");

    ui->A->setValue(pSettings->value("A", 0.0).toDouble());
    ui->B->setValue(pSettings->value("B", 0.0).toDouble());
    ui->C->setValue(pSettings->value("C", 0.0).toDouble());
    ui->T->setValue(pSettings->value("T", 0.0).toDouble());
    ui->coefficients->setChecked(pSettings->value("apply", false).toBool());

    pSettings->endGroup();
}

GraphDialog::~GraphDialog()
{
    delete ui;
}

size_t GraphDialog::getCurrentClusterLenght()
{
    return ui->DataY->currentText().toULongLong();
}

QString GraphDialog::getType()
{
    return ui->DataX->currentText();
}

QString GraphDialog::getClusterSize()
{
    return ui->DataY->currentText();
}

QString GraphDialog::getCurrentWindowGraph()
{
    return  ui->windowGraph->currentText();
}

void GraphDialog::selectLastWindow()
{
    ui->windowGraph->setCurrentIndex(ui->windowGraph->count() - 1);
}

void GraphDialog::clearWindow()
{
    ui->windowGraph->clear();
    ui->windowGraph->addItem(_NEW_WINDOW);
}

void GraphDialog::appendWindow(QString value)
{
    ui->windowGraph->addItem(value);
}

void GraphDialog::slotSelectDataX(QString value)
{
    if(value == ui->DataX->itemText(0))
    {
        ui->DataY->setEnabled(true);
        ui->coefficients->setDisabled(true);
    }
    else if(value == ui->DataX->itemText(1))
    {
        ui->DataY->setDisabled(true);
        ui->coefficients->setDisabled(true);
    }
    else if(value == ui->DataX->itemText(2))
    {
        ui->DataY->setEnabled(true);
        ui->coefficients->setEnabled(true);
    }

    emit signalDataXChanged(value);
}
