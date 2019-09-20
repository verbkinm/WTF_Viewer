#include "rangeaxis.h"

RangeAxis::RangeAxis() :
    _min(std::numeric_limits<double>::max()),
    _max(std::numeric_limits<double>::min())
{

}

void RangeAxis::setMax(double max)
{
    _max = max;
}

void RangeAxis::setMin(double min)
{
    _min = min;
}

double RangeAxis::max()
{
    return _max;
}

double RangeAxis::min()
{
    return _min;
}
