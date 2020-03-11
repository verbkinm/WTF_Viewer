#include "filter_clog.h"

Filter_Clog::Filter_Clog() :
    _frameBegin(0), _frameEnd(0),
    _isAllTotInCluster(false), _isMidiPix(false), _offset(0)
{

}

void Filter_Clog::clear()
{
    _clusterRange.setMin(0);
    _clusterRange.setMax(0);

    _totRange.setMin(0);
    _totRange.setMax(0);

    _isAllTotInCluster = false;
    _isMidiPix = false;
}
