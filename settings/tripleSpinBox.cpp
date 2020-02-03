#include "tripleSpinBox.h"

TripleSpinBox::TripleSpinBox(QWidget *parent)
    : QWidget(parent),
      _maximum(_center.maximum())
{
    _layout.setMargin(0);
    _left.setObjectName("left");
    _center.setObjectName("center");
    _right.setObjectName("right");

    _left.setMinimumWidth(50);
    _center.setMinimumWidth(50);
    _right.setMinimumWidth(50);

    _center.setValue(_center.maximum());

    _layout.addWidget(&_left_label, 0, 0, Qt::AlignCenter);
    _layout.addWidget(&_center_label, 0, 1, Qt::AlignCenter);
    _layout.addWidget(&_right_label, 0, 2, Qt::AlignCenter);

    _layout.addWidget(&_left, 1, 0);
    _layout.addWidget(&_center, 1, 1);
    _layout.addWidget(&_right, 1, 2);

    this->setLayout(&_layout);

    connect(&_left, SIGNAL(valueChanged(int)), SLOT(slotValueChanged()));
    connect(&_center, SIGNAL(valueChanged(int)), SLOT(slotValueChanged()));
    connect(&_right, SIGNAL(valueChanged(int)), SLOT(slotValueChanged()));
}

void TripleSpinBox::setReadOnly(bool left, bool center, bool right)
{
    _left.setButtonSymbols(QAbstractSpinBox::UpDownArrows);
    _center.setButtonSymbols(QAbstractSpinBox::UpDownArrows);
    _right.setButtonSymbols(QAbstractSpinBox::UpDownArrows);

    if(left)
        _left.setButtonSymbols(QAbstractSpinBox::NoButtons);
    if(center)
        _center.setButtonSymbols(QAbstractSpinBox::NoButtons);
    if(right)
        _right.setButtonSymbols(QAbstractSpinBox::NoButtons);

    _left.setReadOnly(left);
    _center.setReadOnly(center);
    _right.setReadOnly(right);
}

void TripleSpinBox::setMaximum(int value)
{
    _left.setMaximum(value);
    _center.setMaximum(value);
    _right.setMaximum(value);

    _left.setValue(0);
    _center.setValue(value);
    _right.setValue(0);
    _maximum = value;
}

void TripleSpinBox::setTitle(const QString &left_title, const QString &center_title, const QString &right_title)
{
    _left_label.setText(left_title);
    _center_label.setText(center_title);
    _right_label.setText(right_title);
}

int TripleSpinBox::getLeftValue() const
{
    return _left.value();
}

int TripleSpinBox::getCenterValue() const
{
    return _center.value();
}

int TripleSpinBox::getRightValue() const
{
    return _right.value();
}

void TripleSpinBox::slotValueChanged()
{
    if(sender()->objectName() == "left")
    {
        _center.setValue(_maximum - _left.value() - _right.value());
        if( (_left.value() + _right.value()) > _maximum)
            _right.setValue(_maximum - _left.value() - _center.value());
    }
    else if(sender()->objectName() == "center")
    {
        _left.setValue(_maximum - _center.value() - _right.value());
        if( (_center.value() + _right.value()) > _maximum)
            _right.setValue(_maximum - _left.value() - _center.value());
    }
    else if(sender()->objectName() == "right")
    {
        _center.setValue(_maximum - _left.value() - _right.value());
        if( (_left.value() + _right.value()) > _maximum)
            _left.setValue(_maximum - _right.value() - _center.value());
    }

    emit signalValueChanged(_left.value(), _center.value(), _right.value());
}


