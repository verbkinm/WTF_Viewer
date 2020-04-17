#include <QImage>
#include <QtMath>
#include <QDebug>

#include "viewer_clog_processor.h"

Viewer_Clog_Processor::Viewer_Clog_Processor() : Viewer_Processor()
{
}

QImage Viewer_Clog_Processor::getImage()
{
    allocateEmptyVec2D(_vec2D, _columns, _rows);

    Filter_Clog filter = _frames.getFilter();
    for (size_t frameNumber = filter._frameBegin - filter._offset; frameNumber <= filter._frameEnd - filter._offset; ++frameNumber)
        for (size_t clusterNumber = 0; clusterNumber < _frames.getClusterCount(frameNumber); ++clusterNumber)
            modifyPointAccordingFilter(frameNumber, clusterNumber);

    rebuildVec2DAccordingToSettings();
    QImage imageFromVec2D = getImageFromVec2D();
    rebuildImageAccordingToSettings(imageFromVec2D);

    return imageFromVec2D;
}

QImage Viewer_Clog_Processor::getRedrawnImage()
{
    rebuildVec2DAccordingToSettings();
    QImage imageFromVec2D = getImageFromVec2D();
    rebuildImageAccordingToSettings(imageFromVec2D);
    return imageFromVec2D;
}

std::pair<float, float> Viewer_Clog_Processor::getClusterRange() const
{
    return {_frames.getClusterMin(), _frames.getClusterMax()};
}

std::vector<size_t> Viewer_Clog_Processor::getClustersLengthVector() const
{
    return _frames.getClustersLengthVector();
}

std::pair<float, float> Viewer_Clog_Processor::getTotsRange() const
{
    return {_frames.getTotMin(), _frames.getTotMax()};
}

std::vector<float> Viewer_Clog_Processor::getVectorOfLengthsOfTots() const
{
    return  _frames.getVectorValueTots();
}

std::pair<float, float> Viewer_Clog_Processor::getTotsSumRange() const
{
    return {_frames.getSumTotMin(), _frames.getSumTotMax()};
}

std::vector<float> Viewer_Clog_Processor::getVectorOfSumOfTots() const
{
    return _frames.getVectorSumTots();
}

const Frames *Viewer_Clog_Processor::getFrames() const
{
    return &_frames;
}

Frames *Viewer_Clog_Processor::getFrames()
{
    return &_frames;
}

void Viewer_Clog_Processor::setFilter(const Filter_Clog &filter)
{
    _frames.setFilter(filter);
}

void Viewer_Clog_Processor::clear()
{
    resetDataToDefault();
}

void Viewer_Clog_Processor::modifyPointAccordingFilter(size_t frameNumber, size_t clusterNumber)
{
    Filter_Clog filter = _frames.getFilter();

    if(!_frames.isClusterInRange(_frames.getClusterLength(frameNumber, clusterNumber), filter._clusterRange))
        return;

    if(filter._isTotRangeChecked && _frames.isTotInRange(frameNumber, clusterNumber, filter._totRange))
    {
        if(filter._isAllTotInCluster)
            modifyPoint(frameNumber, clusterNumber);
        else
        {
            OneFrame::cluster clusterEPoint = _frames.getClusterInTotRange(frameNumber, clusterNumber, filter._totRange);
            for (auto point : clusterEPoint)
                modifyPointAccordingPixMode(&point);
        }
    }
    else if (!filter._isTotRangeChecked && _frames.isSumTotClusterInRange(frameNumber, clusterNumber, filter._totRange))
    {
        modifyPoint(frameNumber, clusterNumber);
    }
}

void Viewer_Clog_Processor::modifyPointAccordingPixMode(OneFrame::ePoint *point)
{
    Filter_Clog filter = _frames.getFilter();

    if(point == nullptr)
        return;

    if(point->x >= _columns || point->y >= _rows )
        return;

    if(filter._isMidiPix)
    {
        setMarkersGeneralOrTot();
        _vec2D.at(point->x).at(point->y) = _vec2D.at(point->x).at(point->y) + 1;
    }
    else
    {
        setMarkersGeneralOrTot();
        _vec2D.at(point->x).at(point->y) = _vec2D.at(point->x).at(point->y) + point->tot;
    }
}

void Viewer_Clog_Processor::modifyPoint(size_t frameNumber, size_t clusterNumber)
{
    for (size_t eventNumber = 0; eventNumber < _frames.getClusterLength(frameNumber, clusterNumber); ++eventNumber)
    {
        auto res = _frames.getEPoint(frameNumber, clusterNumber, eventNumber);
        if(res)
            modifyPointAccordingPixMode(res);
    }
}

void Viewer_Clog_Processor::generalCalibrationSettingsForEPoint(OneFrame::ePoint *point)
{
    if(!checkSettingsPtr() || point == nullptr)
        return;

    double A = (_spSettings->value("GeneralCalibration/A").toDouble());
    double B = (_spSettings->value("GeneralCalibration/B").toDouble());
    double C = (_spSettings->value("GeneralCalibration/C").toDouble());
    double T = (_spSettings->value("GeneralCalibration/T").toDouble());

    double parA = A;
    double parB = B - static_cast<double>(point->tot) - A * T;
    double parC = static_cast<double>(point->tot) * T - B * T - C;

    point->tot = static_cast<float>(( -parB + ( qSqrt(parB * parB - 4 * parA * parC)) ) / (2 * parA));
}

void Viewer_Clog_Processor::setMarkersGeneralOrTot()
{
    if(!checkSettingsPtr())
        return;

    if(_spSettings->value("SettingsClogFile/totMode").toBool())
    {
        _markers |= TOT_MODE;
        _markers &= ~GENERAL_CALIBRATION;
    }
    else if(_spSettings->value("SettingsClogFile/generalCalibration").toBool())
    {
        _markers |= GENERAL_CALIBRATION;
        _markers &= ~TOT_MODE;
    }
}

bool Viewer_Clog_Processor::createVec2D()
{
    if(!_frames.createFromFile(_file))
    {
        clear();
        return false;
    }

    _columns = CLOG_SIZE;
    _rows = CLOG_SIZE;

    Filter_Clog filter = _frames.getFilter();

    if(_spSettings->value("SettingsClogFile/generalCalibration").toBool())
    {
        for (size_t frameNumber = filter._frameBegin - filter._offset; frameNumber <= filter._frameEnd; ++frameNumber)
            for (size_t clusterNumber = 0; clusterNumber < _frames.getClusterCount(frameNumber); ++clusterNumber)
                for (size_t eventNumber = 0; eventNumber < _frames.getClusterLength(frameNumber, clusterNumber); ++eventNumber)
                {
                    auto res = _frames.getEPoint(frameNumber, clusterNumber, eventNumber);
                    if(res)
                        generalCalibrationSettingsForEPoint(res);
                }
    }
    return true;
}

void Viewer_Clog_Processor::resetDataToDefault()
{
    _vec2D.clear();
    _vec2DMask.clear();

    _file.close();
    _fileType = fileType::UNDEFINED;

    _rows = 0;
    _columns = 0;

    _markers = NO_MARKERS;
    _frames.clear();
}
