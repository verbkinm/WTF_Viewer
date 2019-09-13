#include <QKeyEvent>
#include <QDebug>

#include "clogfilterpanel.h"
#include "ui_clogfilterpanel.h"

ClogFilterPanel::ClogFilterPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClogFilterPanel)
{
    ui->setupUi(this);

    ui->clusterRangeGroup->setTitle("Cluster range:");
    ui->clusterRangeGroup->setChecked(true);
    ui->totRangeGroup->setTitle("Tot range:");
    ui->totRangeGroup->setChecked(true);
    ui->totRangeGroupFull->setTitle("Full tot range:");

    connect(ui->apply, SIGNAL(clicked()), SIGNAL(signalApplyFilter()) );
    connect(ui->totRangeGroup, &RangeWidget::signalChecked, ui->totRangeGroupFull, &RangeWidget::setReversChecked);
    connect(ui->totRangeGroupFull, &RangeWidget::signalChecked, ui->totRangeGroup, &RangeWidget::setReversChecked);
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

bool ClogFilterPanel::isClusterEnable() const
{
    return ui->clusterRangeGroup->isChecked();
}

bool ClogFilterPanel::isTotEnable() const
{
    return ui->totRangeGroup->isChecked();
}

bool ClogFilterPanel::isAllTotInCluster() const
{
    return ui->totRangeGroup->isAllTotInCluster();
}

bool ClogFilterPanel::isMediPix() const
{
    return ui->midiPixRadioButton->isChecked();
}

bool ClogFilterPanel::isFullTotRange() const
{
    return ui->totRangeGroupFull->isChecked();
}

void ClogFilterPanel::setAllTotInCluster(bool b)
{
    ui->totRangeGroup->setChecked(b);
}

void ClogFilterPanel::checkedMediPix(bool b)
{
    ui->midiPixRadioButton->setChecked(b);
}

void ClogFilterPanel::checkedTimePix(bool b)
{
    ui->timePixRadioButton->setChecked(b);
}

void ClogFilterPanel::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
        emit signalApplyFilter();
}

