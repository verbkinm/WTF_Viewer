#ifndef FILTER_CLOG_H
#define FILTER_CLOG_H

#include <cstddef>

class Filter_Clog
{
public:
    Filter_Clog();

    size_t _clusterRangeBegin;
    size_t _clusterRangeEnd;

    size_t _totRangeBegin;
    size_t _totRangeEnd;

    size_t _totRangeBeginFull;
    size_t _totRangeEndFull;

    bool _isAllTotInCluster;
    bool _isMidiPix;

    bool _isFullTotRange;

    void clear();
};

#endif // FILTER_CLOG_H
