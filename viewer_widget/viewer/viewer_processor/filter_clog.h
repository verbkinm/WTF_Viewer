#ifndef FILTER_CLOG_H
#define FILTER_CLOG_H

#include <cstddef>

class Filter_Clog
{
public:
    Filter_Clog();

    size_t _clusterRangeBegin;
    size_t _clusterRangeEnd;

    double _totRangeBegin;
    double _totRangeEnd;

//    double _totRangeBeginFull;
//    double _totRangeEndFull;

    bool _isAllTotInCluster;
    bool _isMidiPix;

    bool _isTotRangeChecked;

    void clear();
};

#endif // FILTER_CLOG_H
