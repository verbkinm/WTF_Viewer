#include "pix_filter_panel.h"
#include "ui_pix_filter_panel.h"

Pix_Filter_Panel::Pix_Filter_Panel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Pix_Filter_Panel)
{
    ui->setupUi(this);

    connect(ui->edit_panel, SIGNAL(signalSelectionToggle(bool)), SIGNAL(signalSelectionToggle(bool)));
    connect(ui->edit_panel, SIGNAL(signalPenToggle(bool)), SIGNAL(signalPenToggle(bool)));
    connect(ui->edit_panel, SIGNAL(signalCutClicked(bool)), SIGNAL(signalCutClicked(bool)));
    connect(ui->edit_panel, SIGNAL(signalRepaint()), SIGNAL(signalRepaint()));

    connect(ui->x, SIGNAL(valueChanged(int)), SIGNAL(signalValueX_Changed(int)));
    connect(ui->y, SIGNAL(valueChanged(int)), SIGNAL(signalValueY_Changed(int)));
    connect(ui->width, SIGNAL(valueChanged(int)), SIGNAL(signalValueWidth_Changed(int)));
    connect(ui->heigth, SIGNAL(valueChanged(int)), SIGNAL(signalValueHeight_Changed(int)));

    connect(ui->clogFilterPanel, SIGNAL(signalApplyFilter()), SIGNAL(signalApplyFilter()));
}

Pix_Filter_Panel::Pix_Filter_Panel(const Pix_Filter_Panel &object)
{
    ui->tabWidget->setCurrentIndex(object.ui->tabWidget->currentIndex());
    ui->clogFilterPanel->setCluster(object.ui->clogFilterPanel->isClusterEnable());
    ui->clogFilterPanel->setTot(object.ui->clogFilterPanel->isTotEnable());
    ui->clogFilterPanel->setAllTotInCluster(object.ui->clogFilterPanel->isAllTotInCluster());

    ui->clogFilterPanel->setClusterBegin(static_cast<int>(object.ui->clogFilterPanel->getClusterBegin()));
    ui->clogFilterPanel->setClusterEnd(static_cast<int>(object.ui->clogFilterPanel->getClusterEnd()));

    ui->clogFilterPanel->setTotBegin(static_cast<int>(object.ui->clogFilterPanel->getTotBegin()));
    ui->clogFilterPanel->setTotEnd(static_cast<int>(object.ui->clogFilterPanel->getTotEnd()));

    if(object.ui->clogFilterPanel->isMediPix())
        ui->clogFilterPanel->setMediPix(true);
    else
        ui->clogFilterPanel->setTimePix(true);
}

Pix_Filter_Panel::~Pix_Filter_Panel()
{
    delete ui;
}

void Pix_Filter_Panel::setX(int value)
{
    ui->x->setValue(value);
}

void Pix_Filter_Panel::setY(int value)
{
    ui->y->setValue(value);
}

void Pix_Filter_Panel::setWidth(int value)
{
    ui->width->setValue(value);
}

void Pix_Filter_Panel::setHeight(int value)
{
    ui->heigth->setValue(value);
}

void Pix_Filter_Panel::setDataPanelEnabled(bool state)
{
    ui->data_panel->setEnabled(state);
    ui->edit_panel->buttonCutDisable(!state);
}

void Pix_Filter_Panel::setDataPanelReadOnly(bool state)
{
    ui->x->setReadOnly(state);
    ui->y->setReadOnly(state);
    ui->width->setReadOnly(state);
    ui->heigth->setReadOnly(state);
}

void Pix_Filter_Panel::setToggleButtonEnable(bool state)
{
    ui->edit_panel->setToggleButtons(state);
}

void Pix_Filter_Panel::setButtonCutDisable(bool state)
{
    ui->edit_panel->buttonCutDisable(state);
}

void Pix_Filter_Panel::setTabEnable(int number, bool state)
{
    ui->tabWidget->setTabEnabled(number, state);

}

void Pix_Filter_Panel::setClusterRange(const std::vector<size_t> &vector)
{
    ui->clogFilterPanel->setClusterRange(vector);
}

void Pix_Filter_Panel::setTotRange(const std::vector<double> &vector)
{
    ui->clogFilterPanel->setTotRange(vector);
}

size_t Pix_Filter_Panel::getClusterBegin() const
{
    return ui->clogFilterPanel->getClusterBegin();
}

size_t Pix_Filter_Panel::getClusterEnd() const
{
    return ui->clogFilterPanel->getClusterEnd();
}

size_t Pix_Filter_Panel::getTotBegin() const
{
    return ui->clogFilterPanel->getTotBegin();
}

size_t Pix_Filter_Panel::getTotEnd() const
{
    return ui->clogFilterPanel->getTotEnd();
}

int Pix_Filter_Panel::getX() const
{
    return ui->x->value();
}

int Pix_Filter_Panel::getY() const
{
    return ui->y->value();
}

int Pix_Filter_Panel::getWidth() const
{
    return ui->width->value();
}

int Pix_Filter_Panel::getHeight() const
{
    return ui->heigth->value();
}

QColor Pix_Filter_Panel::getPenColor() const
{
    return ui->edit_panel->getPenColor();
}

double Pix_Filter_Panel::getPenValue() const
{
    return ui->edit_panel->getPenValue();
}

bool Pix_Filter_Panel::isAllTotInCluster() const
{
    return ui->clogFilterPanel->isAllTotInCluster();
}

bool Pix_Filter_Panel::isMediPix() const
{
    return ui->clogFilterPanel->isMediPix();
}

void Pix_Filter_Panel::finishSelection() const
{
    ui->edit_panel->finishSelection();
}

void Pix_Filter_Panel::reset(bool state)
{
    if(!state)
        return;

    ui->x->setValue(0);
    ui->y->setValue(0);
    ui->width->setValue(0);
    ui->heigth->setValue(0);
}
