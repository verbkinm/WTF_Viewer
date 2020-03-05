#include "rangewidget.h"
#include "ui_rangewidget.h"
#include <QDebug>

RangeWidget::RangeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RangeWidget)
{
    ui->setupUi(this);

//    connect(ui->rangeBegin, SIGNAL(valueChanged(double)), SLOT(slotBeginChanged(double)));
//    connect(ui->rangeEnd, SIGNAL(valueChanged(double)), SLOT(slotEndChanged(double)));
}

RangeWidget::~RangeWidget()
{
    delete ui;
}

void RangeWidget::setRange(const std::pair<float, float> &range)
{
    ui->rangeBegin->setRange(range.first, range.second);
    ui->rangeBegin->setValue(range.first);

    ui->rangeEnd->setRange(range.first, range.second);
    ui->rangeEnd->setValue(range.second);
}

void RangeWidget::setTitle(const QString &title)
{
    ui->RangeWidgetGroup->setTitle(title);
}

float RangeWidget::getRangeBegin() const
{
    return ui->rangeBegin->value();
}

float RangeWidget::getRangeEnd() const
{
    return ui->rangeEnd->value();
}

void RangeWidget::slotBeginChanged(double)
{
    disconnect(ui->rangeEnd, SIGNAL(valueChanged(double)), this, SLOT(slotEndChanged(double)));
    ui->rangeEnd->setRange(ui->rangeBegin->value(), ui->rangeEnd->maximum());
    connect(ui->rangeEnd, SIGNAL(valueChanged(double)), SLOT(slotEndChanged(double)));
}

void RangeWidget::slotEndChanged(double)
{
    //!!!
    disconnect(ui->rangeBegin, SIGNAL(valueChanged(double)), this, SLOT(slotBeginChanged(double)));
    ui->rangeBegin->setRange(ui->rangeBegin->minimum(), ui->rangeEnd->value());
    connect(ui->rangeBegin, SIGNAL(valueChanged(double)), SLOT(slotEndChanged(double)));
}
