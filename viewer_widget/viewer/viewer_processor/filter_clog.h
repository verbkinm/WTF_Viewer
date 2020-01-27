#ifndef FILTER_CLOG_H
#define FILTER_CLOG_H

#include <cstddef>
#include <cstdint>
#include "../../../graph/range.h"

class Filter_Clog
{
public:
    Filter_Clog();

    size_t _frameBegin, _frameEnd;
    Range<size_t> _clusterRange;
    Range<double> _totRange;

    bool _isAllTotInCluster;
    bool _isMidiPix;
    bool _isTotRangeChecked;

    void clear();
};

#endif // FILTER_CLOG_H
