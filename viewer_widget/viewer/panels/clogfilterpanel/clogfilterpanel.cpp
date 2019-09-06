#include <QKeyEvent>
#include <QDebug>

#include "clogfilterpanel.h"
#include "ui_clogfilterpanel.h"

ClogFilterPanel::ClogFilterPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClogFilterPanel)
{
    ui->setupUi(this);

    connect(ui->clusterRangeGroup,  SIGNAL(clicked(bool)), this, SLOT(slotEnableRange()));
    connect(ui->totRangeGroup,      SIGNAL(clicked(bool)), this, SLOT(slotEnableRange()));
    connect(ui->totRangeGroupFull, SIGNAL(clicked(bool)), this, SLOT(slotEnableRange()));
    connect(ui->apply,              SIGNAL(clicked()),          this, SLOT(slotApplyFilter()) );
}

ClogFilterPanel::~ClogFilterPanel()
{
    delete ui;
}

void ClogFilterPanel::setClusterRange(const std::vector<size_t> &vector)
{
    disconnectSignals();

    ui->clusterRangeBegin->clear();
    ui->clusterRangeEnd->clear();

    clusterListModel.clear();
    clusterListBegin.clear();
    clusterListEnd.clear();

    for(const auto &value : vector)
        clusterListModel << QString::number(value);

    ui->clusterRangeBegin->addItems(clusterListModel);

    if(ui->clusterRangeGroup->isChecked())
        ui->clusterRangeBegin->setCurrentText(clusterBeginLast);
    else
        ui->clusterRangeBegin->setCurrentText(clusterListModel.first());

    int startFrom = 0;
    for (int i = 0; i < clusterListModel.length(); ++i)
        if(clusterListModel.at(i) == clusterBeginLast)
        {
            startFrom = i;
            break;
        }

    for (int i = startFrom; i < clusterListModel.length(); ++i)
        clusterListEnd << clusterListModel.at(i);

    ui->clusterRangeEnd->clear();
    ui->clusterRangeEnd->addItems(clusterListEnd);

    if(clusterListEnd.contains(clusterEndLast) && ui->clusterRangeGroup->isChecked())
        ui->clusterRangeEnd->setCurrentText(clusterEndLast);
    else
        ui->clusterRangeEnd->setCurrentText(clusterListModel.last());

    connectSignals();
}
void ClogFilterPanel::slotDates(QString value)
{
    if(value.isEmpty())
        return;

    QComboBox* pCB = static_cast<QComboBox*>(sender());
    QString currentText;
    if(pCB->objectName() == "clusterRangeBegin")
    {
        disconnectSignals();

        clusterListEnd.clear();

        int startFrom = 0;
        for (int i = 0; i < clusterListModel.length(); ++i)
            if(clusterListModel.at(i) == value)
            {
                startFrom = i;
                break;
            }

        for (int i = startFrom; i < clusterListModel.length(); ++i)
            clusterListEnd << clusterListModel.at(i);

        ui->clusterRangeEnd->clear();
        ui->clusterRangeEnd->addItems(clusterListEnd);

        if(clusterListEnd.contains(clusterEndLast))
            ui->clusterRangeEnd->setCurrentText(clusterEndLast);
        else
            ui->clusterRangeEnd->setCurrentText(clusterListModel.last());

        if(ui->clusterRangeGroup->isChecked())
            clusterBeginLast = value;

        connectSignals();
    }
    else if(pCB->objectName() == "clusterRangeEnd")
    {
        disconnectSignals();

        if(ui->clusterRangeGroup->isChecked())
            clusterEndLast = value;

        connectSignals();
    }
    else if(pCB->objectName() == "totRangeBegin")
    {
        disconnectSignals();

        totListEnd.clear();

        int startFrom = 0;
        for (int i = 0; i < totListModel.length(); ++i)
            if(totListModel.at(i) == value)
            {
                startFrom = i;
                break;
            }

        for (int i = startFrom; i < totListModel.length(); ++i)
            totListEnd << totListModel.at(i);

        ui->totRangeEnd->clear();
        ui->totRangeEnd->addItems(totListEnd);

        if(totListEnd.contains(totEndLast))
            ui->totRangeEnd->setCurrentText(totEndLast);
        else
            ui->totRangeEnd->setCurrentText(totListModel.last());

        if(ui->totRangeGroup->isChecked())
            totBeginLast = value;

        connectSignals();
    }
    else if(pCB->objectName() == "totRangeEnd")
    {
        disconnectSignals();

        if(ui->totRangeGroup->isChecked())
            totEndLast = value;

        connectSignals();
    }
    else if(pCB->objectName() == "totRangeBeginFull")
    {
        disconnectSignals();

        totListEndFull.clear();

        int startFrom = 0;
        for (int i = 0; i < totListModelFull.length(); ++i)
            if(totListModelFull.at(i) == value)
            {
                startFrom = i;
                break;
            }

        for (int i = startFrom; i < totListModelFull.length(); ++i)
            totListEndFull << totListModelFull.at(i);

        ui->totRangeEndFull->clear();
        ui->totRangeEndFull->addItems(totListEndFull);

        if(totListEndFull.contains(totEndLastFull))
            ui->totRangeEndFull->setCurrentText(totEndLastFull);
        else
            ui->totRangeEndFull->setCurrentText(totListModelFull.last());

        if(ui->totRangeGroupFull->isChecked())
            totBeginLastFull = value;

        connectSignals();
    }
    else if(pCB->objectName() == "totRangeEndFull")
    {
        disconnectSignals();

        if(ui->totRangeGroupFull->isChecked())
            totEndLastFull = value;

        connectSignals();
    }
}

void ClogFilterPanel::slotApplyFilter()
{
    if(ui->clusterRangeGroup->isChecked())
    {
        clusterBeginLast = ui->clusterRangeBegin->currentText();
        clusterEndLast   = ui->clusterRangeEnd->currentText();
    }
    if(ui->totRangeGroup->isChecked())
    {
        totBeginLast = ui->totRangeBegin->currentText();
        totEndLast   = ui->totRangeEnd->currentText();
    }
    if(ui->totRangeGroupFull->isChecked())
    {
        totBeginLastFull = ui->totRangeBeginFull->currentText();
        totEndLastFull = ui->totRangeEndFull->currentText();
    }

    emit signalApplyFilter();
}

void ClogFilterPanel::setTotRange(const std::vector<double> &vector)
{
    disconnectSignals();

    ui->totRangeBegin->clear();
    ui->totRangeEnd->clear();

    totListModel.clear();
    totListBegin.clear();
    totListEnd.clear();

    for(const auto &value : vector)
        totListModel << QString::number(value);

    ui->totRangeBegin->addItems(totListModel);

    if(ui->totRangeGroup->isChecked())
        ui->totRangeBegin->setCurrentText(totBeginLast);
    else
        ui->totRangeBegin->setCurrentText(totListModel.first());

    int startFrom = 0;
    for (int i = 0; i < totListModel.length(); ++i)
        if(totListModel.at(i) == totBeginLast)
        {
            startFrom = i;
            break;
        }

    for (int i = startFrom; i < totListModel.length(); ++i)
        totListEnd << totListModel.at(i);

    ui->totRangeEnd->clear();
    ui->totRangeEnd->addItems(totListEnd);

    if(totListEnd.contains(totEndLast) && ui->totRangeGroup->isChecked())
        ui->totRangeEnd->setCurrentText(totEndLast);
    else
        ui->totRangeEnd->setCurrentText(totListModel.last());

    connectSignals();
}

void ClogFilterPanel::setTotRangeFull(const std::vector<double> &vector)
{
    disconnectSignals();

    ui->totRangeBeginFull->clear();
    ui->totRangeEndFull->clear();

    totListModelFull.clear();
    totListBeginFull.clear();
    totListEndFull.clear();

    for(const auto &value : vector)
        totListModelFull << QString::number(value);

    ui->totRangeBeginFull->addItems(totListModelFull);

    if(ui->totRangeGroupFull->isChecked())
        ui->totRangeBeginFull->setCurrentText(totBeginLastFull);
    else
        ui->totRangeBeginFull->setCurrentText(totListModelFull.first());

    int startFrom = 0;
    for (int i = 0; i < totListModelFull.length(); ++i)
        if(totListModelFull.at(i) == totBeginLastFull)
        {
            startFrom = i;
            break;
        }

    for (int i = startFrom; i < totListModelFull.length(); ++i)
        totListEndFull << totListModelFull.at(i);

    ui->totRangeEndFull->clear();
    ui->totRangeEndFull->addItems(totListEndFull);

    if(totListEndFull.contains(totEndLastFull) && ui->totRangeGroupFull->isChecked())
        ui->totRangeEndFull->setCurrentText(totEndLastFull);
    else
        ui->totRangeEndFull->setCurrentText(totListModelFull.last());

    connectSignals();
}

size_t ClogFilterPanel::getClusterBegin() const
{
    return ui->clusterRangeBegin->currentText().toULongLong();
}

size_t ClogFilterPanel::getClusterEnd() const
{
    return ui->clusterRangeEnd->currentText().toULongLong();
}

size_t ClogFilterPanel::getTotBegin() const
{
    return ui->totRangeBegin->currentText().toULongLong();
}

size_t ClogFilterPanel::getTotEnd() const
{
    return ui->totRangeEnd->currentText().toULongLong();
}

size_t ClogFilterPanel::getTotBeginFull() const
{
    return ui->totRangeBeginFull->currentText().toULongLong();
}

size_t ClogFilterPanel::getTotEndFull() const
{
    return ui->totRangeEndFull->currentText().toULongLong();
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
    return ui->allTotInCluster->isChecked();
}

bool ClogFilterPanel::isMediPix() const
{
    return ui->midiPixRadioButton->isChecked();
}

bool ClogFilterPanel::isFullTotRange() const
{
    return ui->totRangeGroupFull->isChecked();
}

void ClogFilterPanel::setClusterBegin(int v)
{
    ui->clusterRangeBegin->setCurrentText(QString::number(v));
}

void ClogFilterPanel::setClusterEnd(int v)
{
    ui->clusterRangeEnd->setCurrentText(QString::number(v));
}

void ClogFilterPanel::setTotBegin(int v)
{
    ui->totRangeBegin->setCurrentText(QString::number(v));
}

void ClogFilterPanel::setTotEnd(int v)
{
    ui->totRangeEnd->setCurrentText(QString::number(v));
}

void ClogFilterPanel::setTotBeginFull(int v)
{
    ui->totRangeBeginFull->setCurrentText(QString::number(v));
}

void ClogFilterPanel::setTotEndFull(int v)
{
    ui->totRangeEndFull->setCurrentText(QString::number(v));
}

void ClogFilterPanel::enableClusterGroup(bool b)
{
    ui->clusterRangeGroup->setChecked(b);
}

void ClogFilterPanel::enableTotGroup(bool b)
{
    ui->totRangeGroup->setChecked(b);
}

void ClogFilterPanel::setAllTotInCluster(bool b)
{
    ui->allTotInCluster->setChecked(b);
}

void ClogFilterPanel::checkedMediPix(bool b)
{
    ui->midiPixRadioButton->setChecked(b);
}

void ClogFilterPanel::checkedTimePix(bool b)
{
    ui->timePixRadioButton->setChecked(b);
}

void ClogFilterPanel::disconnectSignals() const
{
    disconnect(ui->clusterRangeBegin,  SIGNAL(currentTextChanged(QString)), this, SLOT(slotDates(QString)));
    disconnect(ui->clusterRangeEnd,    SIGNAL(currentTextChanged(QString)), this, SLOT(slotDates(QString)));

    disconnect(ui->totRangeBegin,  SIGNAL(currentTextChanged(QString)), this, SLOT(slotDates(QString)));
    disconnect(ui->totRangeEnd,    SIGNAL(currentTextChanged(QString)), this, SLOT(slotDates(QString)));

    disconnect(ui->totRangeBeginFull,  SIGNAL(currentTextChanged(QString)), this, SLOT(slotDates(QString)));
    disconnect(ui->totRangeEndFull,    SIGNAL(currentTextChanged(QString)), this, SLOT(slotDates(QString)));
}

void ClogFilterPanel::connectSignals() const
{
    connect(ui->clusterRangeBegin,  SIGNAL(currentTextChanged(QString)), SLOT(slotDates(QString)));
    connect(ui->clusterRangeEnd,  SIGNAL(currentTextChanged(QString)), SLOT(slotDates(QString)));

    connect(ui->totRangeBegin,  SIGNAL(currentTextChanged(QString)), SLOT(slotDates(QString)));
    connect(ui->totRangeEnd,    SIGNAL(currentTextChanged(QString)), SLOT(slotDates(QString)));

    connect(ui->totRangeBeginFull,  SIGNAL(currentTextChanged(QString)), SLOT(slotDates(QString)));
    connect(ui->totRangeEndFull,    SIGNAL(currentTextChanged(QString)), SLOT(slotDates(QString)));
}

void ClogFilterPanel::slotEnableRange()
{
    QGroupBox* groupBox = static_cast<QGroupBox*>(sender());

    if(groupBox->objectName() == "clusterRangeGroup")
    {
        if(groupBox->isChecked())
        {
            ui->clusterRangeBegin->setEnabled(true);
            ui->clusterRangeEnd->setEnabled(true);

            ui->clusterRangeBegin->setCurrentText(clusterBeginLast);
            ui->clusterRangeEnd->setCurrentText(clusterEndLast);
        }
        else
        {
            ui->clusterRangeBegin->setEnabled(false);
            ui->clusterRangeEnd->setEnabled(false);

            ui->clusterRangeBegin->setCurrentText(clusterListModel.first());
            ui->clusterRangeEnd->setCurrentText(clusterListModel.last());
        }
    }
    else if(groupBox->objectName() == "totRangeGroup")
    {
        ui->totRangeGroupFull->setChecked(false);
        if(groupBox->isChecked())
        {
            ui->totRangeBegin->setEnabled(true);
            ui->totRangeEnd->setEnabled(true);

            ui->totRangeBegin->setCurrentText(totBeginLast);
            ui->totRangeEnd->setCurrentText(totEndLast);

            ui->allTotInCluster->setEnabled(true);
        }
        else
        {
            ui->totRangeBegin->setEnabled(false);
            ui->totRangeEnd->setEnabled(false);

            ui->totRangeBegin->setCurrentText(totListModel.first());
            ui->totRangeEnd->setCurrentText(totListModel.last());

            ui->allTotInCluster->setEnabled(false);
            if(!ui->allTotInCluster->isChecked())
                ui->allTotInCluster->toggle();
        }
    }
    else if(groupBox->objectName() == "totRangeGroupFull")
    {
        ui->totRangeGroup->setChecked(false);
    }
}

void ClogFilterPanel::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
        emit signalApplyFilter();
}

