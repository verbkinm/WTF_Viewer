#include "rangewidget.h"
#include "ui_rangewidget.h"
#include <QDebug>

RangeWidget::RangeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RangeWidget)
{
    ui->setupUi(this);
}

RangeWidget::~RangeWidget()
{
    delete ui;
}

void RangeWidget::setRange(const std::vector<double> &vector)
{
    disconnectSignals();

    if(!vector.size())
        return;

    _vector = vector;
    std::sort(_vector.begin(), _vector.end());

    if(_beginLast.isEmpty() || !containsInVector(_beginLast))
        _beginLast = QString::number(_vector[0]);
    if(_endLast.isEmpty() || !containsInVector(_endLast))
        _endLast = QString::number(*_vector.rbegin());

    fillBeginComboBox();
    fillEndComboBox();

    connectSignals();
}

double RangeWidget::getRangeBegin() const
{
    return ui->rangeBegin->currentText().toULongLong();
}

double RangeWidget::getRangeEnd() const
{
    return ui->rangeEnd->currentText().toULongLong();
}

QGridLayout *RangeWidget::layout()
{
    return ui->gridLayout;
}

void RangeWidget::fillBeginComboBox()
{
    ui->rangeBegin->clear();
    for(const auto &item : _vector)
        ui->rangeBegin->addItem(QString::number(item));

    if(containsInVector(_beginLast))
        ui->rangeBegin->setCurrentText(_beginLast);
    else
        ui->rangeBegin->setCurrentIndex(0);
}

void RangeWidget::fillEndComboBox()
{
    ui->rangeEnd->clear();
    auto it = std::find(_vector.begin(), _vector.end(), _beginLast.toULongLong());
    while (it != _vector.end())
    {
        ui->rangeEnd->addItem(QString::number(*it));
        it++;
    }

    if(containsInVector(_endLast))
        ui->rangeEnd->setCurrentText(_endLast);
    else
        ui->rangeEnd->setCurrentText(QString::number(*_vector.rbegin()));
}
bool RangeWidget::containsInVector(QString str_value)
{
    double digit_value = str_value.toULongLong();
    auto it = std::find(_vector.begin(), _vector.end(), digit_value);
    if(it != _vector.end())
        return true;

    return false;
}

void RangeWidget::disconnectSignals()
{
    disconnect(ui->rangeBegin, SIGNAL(currentTextChanged(QString)), this, SLOT(slotChangeBeginRange(QString)));
    disconnect(ui->rangeEnd, SIGNAL(currentTextChanged(QString)), this, SLOT(slotChangeEndRange(QString)));
}

void RangeWidget::connectSignals()
{
    connect(ui->rangeBegin, SIGNAL(currentTextChanged(QString)), this, SLOT(slotChangeBeginRange(QString)));
    connect(ui->rangeEnd, SIGNAL(currentTextChanged(QString)), this, SLOT(slotChangeEndRange(QString)));
}
void RangeWidget::slotChangeEndRange(QString currentText)
{
    _endLast = currentText;
}

//void RangeWidget::slotRangeGroup(bool checked)
//{
//    disconnectSignals();
//    if(checked)
//    {
//        ui->rangeBegin->setCurrentText(_beginLast);
//        ui->rangeEnd->setCurrentText(_endLast);
//    }
//    else
//    {
//        ui->rangeBegin->setCurrentIndex(0);
//        ui->rangeEnd->setCurrentText(QString::number(*_vector.rbegin()));
//    }
//    connectSignals();
//}

void RangeWidget::slotChangeBeginRange(QString currentText)
{
    disconnect(ui->rangeEnd, SIGNAL(currentTextChanged(QString)), this, SLOT(slotChangeEndRange(QString)));

    _beginLast = currentText;
    fillEndComboBox();

    connect(ui->rangeEnd, SIGNAL(currentTextChanged(QString)), this, SLOT(slotChangeEndRange(QString)));
}
