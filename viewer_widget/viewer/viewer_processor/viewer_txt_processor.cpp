#include <QImage>

#include "../../checkFile/checkfile.h"
#include "viewer_txt_processor.h"

Viewer_Txt_Processor::Viewer_Txt_Processor() : Viewer_Processor()
{

}

QImage Viewer_Txt_Processor::getImage()
{
    rebuildVec2DAccordingToSettings();
    QImage imageFromVec2D = getImageFromVec2D();
    rebuildImageAccordingToSettings(imageFromVec2D);
    return imageFromVec2D;
}

QImage Viewer_Txt_Processor::getRedrawnImage()
{
    return getImage();
}

void Viewer_Txt_Processor::clear()
{
    resetDataToDefault();
}

//void Viewer_Txt_Processor::rebuildVec2DAccordingToSettings()
//{
//    if(!checkSettingsPtr())
//        return;
//    if(_pSettings->value("SettingsImage/FrameGroupBox").toBool())
//    {
//        createFrameInVec2D();
//        _markers |= BORDER;
//    }
//    else
//        _markers &= ~BORDER;
//    if(_pSettings->value("SettingsImage/MasquradingGroupBox").toBool())
//    {
//        createMaskInVec2D();
//        _markers |= MASKING;
//    }
//    else
//        _markers &= ~MASKING;
//}

//void Viewer_Txt_Processor::rebuildImageAccordingToSettings(QImage &image)
//{
//    if(!checkSettingsPtr())
//        return;
//    if(_pSettings->value("SettingsImage/MasquradingGroupBox").toBool())
//    {
//    //рисуем маскированые пиксели выбраным цветом
//    for (size_t  x = 0; x < _columns; ++x)
//        for (size_t  y = 0; y < _rows; ++y)
//            if(_vec2DMask.at(x).at(y) > 0)
//                image.setPixelColor(static_cast<int>(x), static_cast<int>(y), QColor(_pSettings->value("SettingsImage/maskColor").toString()));
//    }
//}

bool Viewer_Txt_Processor::createVec2D()
{
    VectorFromTxtFile vectorFromTxtFile(QFileInfo(_file).absoluteFilePath());
    _columns = vectorFromTxtFile._column;
    _rows = vectorFromTxtFile._row;

    allocateEmptyVec2D(_vec2D, _columns, _rows);

    size_t i = 0;
    for(size_t  y = 0; y < _rows; ++y)
    {
        for(size_t  x = 0; x < _columns; ++x)
        {
            float value = vectorFromTxtFile._vector[i++];
            _vec2D.at(x).at(y) = value;
        }
    }
    return true;
}

void Viewer_Txt_Processor::resetDataToDefault()
{
    _vec2D.clear();
    _vec2DMask.clear();
    _file.close();
    _rows = 0;
    _columns = 0;
    _fileType = fileType::UNDEFINED;
    _markers = NO_MARKERS;
}
