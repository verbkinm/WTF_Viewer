#ifndef FILTER_CLOG_H
#define FILTER_CLOG_H

#include <cstddef>

class Filter_Clog
{
public:
    Filter_Clog();

    double _clusterRangeBegin;
    double _clusterRangeEnd;

    double _totRangeBegin;
    double _totRangeEnd;

    double _totRangeBeginFull;
    double _totRangeEndFull;

    bool _isAllTotInCluster;
    bool _isMidiPix;

    bool _isFullTotRange;

    void clear();
};

#endif // FILTER_CLOG_H
