#ifndef RANGE_H
#define RANGE_H

#include <limits>

template <typename T>
class Range
{
public:
    Range() :
        _min(std::numeric_limits<T>::max()),
        _max(std::numeric_limits<T>::min())
    {

    }

    void setMax(T max)
    {
        _max = max;
    }

    void setMin(T min)
    {
        _min = min;
    }

    T max()
    {
        return _max;
    }

    T min()
    {
        return _min;
    }

private:
    T _min;
    T _max;
};

#endif // RANGE_H
