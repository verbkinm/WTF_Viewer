#ifndef RANGEWIDGET_EXTENDED_H
#define RANGEWIDGET_EXTENDED_H

#include "rangewidget.h"
#include <QWidget>
#include <QCheckBox>

class RangeWidget_Extended : public RangeWidget
{
public:
    RangeWidget_Extended(QWidget* parent);

    bool isAllTotInCluster();

private:
    QCheckBox _allTotInCluster;
};

#endif // RANGEWIDGET_EXTENDED_H
