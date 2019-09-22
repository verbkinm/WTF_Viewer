#include "filter_clog.h"

Filter_Clog::Filter_Clog() :
//    _clusterRangeBegin(0), _clusterRangeEnd(0),
//    _totRangeBegin(0), _totRangeEnd(0),
    _isAllTotInCluster(false), _isMidiPix(false)
{

}

void Filter_Clog::clear()
{
    _clusterRange.setMin(0);
    _clusterRange.setMax(0);
//    _clusterRangeBegin = 0;
//    _clusterRangeEnd = 0;
    _totRange.setMin(0);
    _totRange.setMax(0);
//    _totRangeBegin = 0;
//    _totRangeEnd = 0;
    _isAllTotInCluster = false;
    _isMidiPix = false;
}
