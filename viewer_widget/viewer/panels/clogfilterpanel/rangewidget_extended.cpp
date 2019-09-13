#include "rangewidget_extended.h"

RangeWidget_Extended::RangeWidget_Extended(QWidget *parent) : RangeWidget (parent)
{
    _allTotInCluster.setText("all Tot in cluster");
    _allTotInCluster.setChecked(true);
    layout()->addWidget(&_allTotInCluster);
}

bool RangeWidget_Extended::isAllTotInCluster()
{
    return _allTotInCluster.isChecked();
}
