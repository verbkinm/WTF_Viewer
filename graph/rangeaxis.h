#ifndef RANGEAXIS_H
#define RANGEAXIS_H
#include <limits>

class RangeAxis
{
public:
    RangeAxis();

    void setMax(double max);
    void setMin(double min);

    double max();
    double min();

private:
    double _min;
    double _max;
};

#endif // RANGEAXIS_H
