#ifndef FILTER_CLOG_H
#define FILTER_CLOG_H

#include <cstddef>
#include "../../../graph/range.h"

class Filter_Clog
{
public:
    Filter_Clog();

    Range<size_t> _clusterRange;
//    size_t _clusterRangeBegin;
//    size_t _clusterRangeEnd;

    Range<double> _totRange;
//    double _totRangeBegin;
//    double _totRangeEnd;

//    double _totRangeBeginFull;
//    double _totRangeEndFull;

    bool _isAllTotInCluster;
    bool _isMidiPix;

    bool _isTotRangeChecked;

    void clear();
};

#endif // FILTER_CLOG_H
