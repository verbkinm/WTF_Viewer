#include <QImage>
#include <QtMath>
#include "viewer_clog_processor.h"

Viewer_Clog_Processor::Viewer_Clog_Processor() : Viewer_Processor()
{

}

QImage Viewer_Clog_Processor::getImage()
{
    allocateEmptyVec2D(_vec2D, _columns, _rows);

    for (size_t frameNumber = 0; frameNumber < _frames.getFrameCount(); ++frameNumber)
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

std::vector<double> Viewer_Clog_Processor::getClustersLengthVector() const
{
    return _frames.getClustersLengthVector();
}

std::vector<double> Viewer_Clog_Processor::getVectorOfLengthsOfTots() const
{
    return  _frames.getVectorValueTots();
}

std::vector<double> Viewer_Clog_Processor::getVectorOfSumOfTots() const
{
    return _frames.getVectorSumTots();
}

Frames const &Viewer_Clog_Processor::getFrames() const
{
    return _frames;
}

void Viewer_Clog_Processor::setFilter(const Filter_Clog &filter)
{
    _filter = filter;
}

void Viewer_Clog_Processor::modifyPointAccordingFilter(size_t frameNumber, size_t clusterNumber)
{
    if(!_frames.isClusterInRange(_frames.getClusterLength(frameNumber, clusterNumber), _filter._clusterRange))
        return;

    if(_filter._isTotRangeChecked && _frames.isTotInRange(frameNumber, clusterNumber, _filter._totRange))
    {
        if(_filter._isAllTotInCluster)
            modifyPoint(frameNumber, clusterNumber);
        else
        {
            OneFrame::cluster clusterEPoint = _frames.getClusterInTotRange(frameNumber, clusterNumber, _filter._totRange);
            for (auto &point : clusterEPoint)
                modifyPointAccordingPixMode(point);
        }
    }
    else if (!_filter._isTotRangeChecked && _frames.isSumTotClusterInRange(frameNumber, clusterNumber, _filter._totRange))
    {
        modifyPoint(frameNumber, clusterNumber);
    }
}

void Viewer_Clog_Processor::modifyPointAccordingPixMode(OneFrame::ePoint &point)
{
    if(point.x >= _columns || point.y >= _rows )
        return;

    if(_filter._isMidiPix)
    {
        setMarkersGeneralOrTot();
        _vec2D.at(point.x).at(point.y) = _vec2D.at(point.x).at(point.y) + 1;
    }
    else
    {
        setMarkersGeneralOrTot();
        _vec2D.at(point.x).at(point.y) = _vec2D.at(point.x).at(point.y) + point.tot;
    }
}

void Viewer_Clog_Processor::modifyPoint(size_t frameNumber, size_t clusterNumber)
{
    for (size_t eventNumber = 0; eventNumber < _frames.getClusterLength(frameNumber, clusterNumber); ++eventNumber)
    {
        OneFrame::ePoint point = _frames.getEPoint(frameNumber, clusterNumber, eventNumber);
        modifyPointAccordingPixMode(point);
    }
}

void Viewer_Clog_Processor::generalCalibrationSettingsForEPoint(OneFrame::ePoint &point)
{
    if(!checkSettingsPtr())
        return;

    double A = (_spSettings->value("GeneralCalibration/A").toDouble());
    double B = (_spSettings->value("GeneralCalibration/B").toDouble());
    double C = (_spSettings->value("GeneralCalibration/C").toDouble());
    double T = (_spSettings->value("GeneralCalibration/T").toDouble());

    double parA = A;
    double parB = B - point.tot - A * T;
    double parC = point.tot * T - B * T - C;

    point.tot = ( -parB + ( qSqrt(parB * parB - 4 * parA * parC)) ) / (2 * parA);
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

void Viewer_Clog_Processor::createVec2D()
{
    _frames.createFromFile(_fileName);
    _columns  = CLOG_SIZE;
    _rows     = CLOG_SIZE;

    if(_spSettings->value("SettingsClogFile/generalCalibration").toBool())
    {
        for (size_t frameNumber = 0; frameNumber < _frames.getFrameCount(); ++frameNumber)
            for (size_t clusterNumber = 0; clusterNumber < _frames.getClusterCount(frameNumber); ++clusterNumber)
                for (size_t eventNumber = 0; eventNumber < _frames.getClusterLength(frameNumber, clusterNumber); ++eventNumber)
                    generalCalibrationSettingsForEPoint(_frames.getPointer_to_EPoint(frameNumber, clusterNumber, eventNumber));
    }
}

void Viewer_Clog_Processor::resetDataToDefault()
{
    _vec2D.clear();
    _vec2DMask.clear();

    _fileName.clear();
    _fileType = fileType::UNDEFINED;

    _rows = 0;
    _columns = 0;

    _markers = NO_MARKERS;
    _frames.clear();
}
