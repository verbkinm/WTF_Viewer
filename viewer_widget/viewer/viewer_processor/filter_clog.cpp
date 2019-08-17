#include "filter_clog.h"

Filter_Clog::Filter_Clog() : _clusterRangeBegin(0), _clusterRangeEnd(0),
    _totRangeBegin(0), _totRangeEnd(0),
    _isAllTotInCluster(false), _isMidiPix(false)
{

}

void Filter_Clog::clear()
{
    _clusterRangeBegin = 0;
    _clusterRangeEnd = 0;
    _totRangeBegin = 0;
    _totRangeEnd = 0;
    _isAllTotInCluster = false;
    _isMidiPix = false;
}
