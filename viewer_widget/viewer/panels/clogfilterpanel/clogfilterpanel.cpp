#include <QKeyEvent>
#include <QDebug>

#include "clogfilterpanel.h"
#include "ui_clogfilterpanel.h"

ClogFilterPanel::ClogFilterPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClogFilterPanel)
{
    ui->setupUi(this);

    connect(ui->apply, SIGNAL(clicked()), SIGNAL(signalApplyFilter()) );
    connect(ui->totRange, &QAbstractButton::clicked, this, &ClogFilterPanel::slotTotType);
    connect(ui->totRangeFull, &QAbstractButton::clicked, this, &ClogFilterPanel::slotTotType);

    ui->clusterRangeGroup->setTitle("Clusters:");
    ui->totRangeGroup->setTitle("Tots/Energy:");
    ui->totRangeGroupFull->setTitle("Tots/Energy:");
}

ClogFilterPanel::~ClogFilterPanel()
{
    delete ui;
}

void ClogFilterPanel::setClusterRange(const std::vector<double> &vector)
{
    ui->clusterRangeGroup->setRange(vector);
}

void ClogFilterPanel::setTotRange(const std::vector<double> &vector)
{
    ui->totRangeGroup->setRange(vector);
}

void ClogFilterPanel::setTotRangeFull(const std::vector<double> &vector)
{
    ui->totRangeGroupFull->setRange(vector);
}

double ClogFilterPanel::getClusterBegin() const
{
    return ui->clusterRangeGroup->getRangeBegin();
}

double ClogFilterPanel::getClusterEnd() const
{
    return ui->clusterRangeGroup->getRangeEnd();
}

double ClogFilterPanel::getTotBegin() const
{
    return ui->totRangeGroup->getRangeBegin();
}

double ClogFilterPanel::getTotEnd() const
{
    return  ui->totRangeGroup->getRangeEnd();
}

double ClogFilterPanel::getTotBeginFull() const
{
    return ui->totRangeGroupFull->getRangeBegin();
}

double ClogFilterPanel::getTotEndFull() const
{
    return ui->totRangeGroupFull->getRangeEnd();
}

bool ClogFilterPanel::isAllTotInCluster() const
{
    return ui->totRangeGroup->isAllTotInCluster();
}

bool ClogFilterPanel::isMediPix() const
{
    return ui->midiPixRadioButton->isChecked();
}

bool ClogFilterPanel::isTotRangeChecked() const
{
    return ui->totRange->isChecked();
}

void ClogFilterPanel::checkedMediPix(bool b)
{
    ui->midiPixRadioButton->setChecked(b);
}

void ClogFilterPanel::checkedTimePix(bool b)
{
    ui->timePixRadioButton->setChecked(b);
}

void ClogFilterPanel::slotTotType()
{
    QObject* obj = sender();
    if(obj->objectName() == "totRange")
        ui->stackedWidget->setCurrentIndex(0);
    else
        ui->stackedWidget->setCurrentIndex(1);
}

void ClogFilterPanel::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
        emit signalApplyFilter();
}


