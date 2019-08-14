#include <QFileInfo>
#include <QImage>

#include "viewer_widget/checkFile/checkfile.h"
#include "viewer_processor.h"

Viewer_Processor::Viewer_Processor() : _fileType(fileType::UNDEFINED),
    _rows(0), _columns(0)
{

}

QString Viewer_Processor::getFileName() const
{
    return _fileName;
}

double Viewer_Processor::getDataInVec2D(size_t column, size_t row)
{
    try
    {
        return _vec2D.at(column).at(row);
    }
    catch (std::out_of_range &)
    {
        return 0;
    }
}

size_t Viewer_Processor::getColumns()
{
    return _columns;
}

size_t Viewer_Processor::getRows()
{
    return _rows;
}

void Viewer_Processor::setDataInVec2D(size_t column, size_t row, double value)
{
    _vec2D.at(column).at(row) = value;
}

void Viewer_Processor::setFileName(const QString &fileName)
{
    resetDataToDefault();
    _fileName = fileName;
    setFileType(fileName);
    createVec2D();
}

void Viewer_Processor::setSettings(QSettings* const settings)
{
    _settings = settings;
}

QImage Viewer_Processor::getImage()
{
    rebuildVec2DAccordingToSettings();
    QImage imageFromVec2D = getImageFromVec2D();
    rebuildImageAccordingToSettings(imageFromVec2D);
    return imageFromVec2D;
}

void Viewer_Processor::cutVec2D(int cutX, int cutY, size_t width, size_t height)
{
    std::vector<std::vector<double>> tmpVec2D(width, std::vector<double>(height));

    double value = 0;
    for (int x = cutX, tmpX = 0; tmpX < static_cast<int>(width); ++x, ++tmpX)
    {
        for (int y = cutY, tmpY = 0; tmpY < static_cast<int>(height); ++y, ++tmpY) {
            if( (x < 0) || (x >= static_cast<int>(_columns)) || (y < 0) || (y >= static_cast<int>(_rows)) )
                value = 0;
            else
                value = _vec2D.at(static_cast<size_t>(x)).at(static_cast<size_t>(y));
            tmpVec2D.at(static_cast<size_t>(tmpX)).at(static_cast<size_t>(tmpY)) = value;
        }
    }

    _vec2D = tmpVec2D;
    _columns = width;
    _rows = height;
}

QImage Viewer_Processor::getImageFromVec2D()
{
    double max = findMaxInArrayOrigin();
    double min = findMinInArrayOrigin();

    QImage image(static_cast<int>(_columns), static_cast<int>(_rows), QImage::Format_ARGB32_Premultiplied);
    for (size_t  x = 0; x < _columns; ++x)
    {
        for (size_t  y = 0; y < _rows; ++y)
        {
            double value = convert(_vec2D.at(x).at(y), \
                                    min, \
                                    max, \
                                    double(0), \
                                    double(255) );

            QColor color(qRound(value), qRound(value), qRound(value));
            image.setPixelColor(static_cast<int>(x), static_cast<int>(y), color);
        }
    }
    return image;
}

bool Viewer_Processor::checkSettingsPtr()
{
    if(_settings != nullptr)
        return true;

    qDebug() << __FUNCTION__ << " _settings = nullptr";
    return false;
}

void Viewer_Processor::setFileType(const QString &fileName)
{
    QFileInfo file(fileName);
    if(file.suffix() == "txt")
        _fileType= fileType::TXT;
    else if(file.suffix() == "clog")
        _fileType = fileType::CLOG;
    else
        _fileType = fileType::UNDEFINED;
}

void Viewer_Processor::allocateEmptyVec2D(std::vector<std::vector<double>> &vec2D, size_t columns, size_t rows)
{
    vec2D.resize(columns, std::vector<double>(rows, 0));
}

void Viewer_Processor::createVec2D()
{
    if(_fileType == fileType::TXT)
        createVec2DFromTXT();
    else if(_fileType == fileType::CLOG)
        createVec2DFromTXT();
}

void Viewer_Processor::createVec2DFromTXT()
{
    VectorFromTxtFile vectorFromTxtFile(_fileName);
    _columns  = vectorFromTxtFile._column;
    _rows     = vectorFromTxtFile._row;

    allocateEmptyVec2D(_vec2D, _columns, _rows);

    int i = 0;
    for(size_t  y = 0; y < _rows; ++y)
    {
        for(size_t  x = 0; x < _columns; ++x)
        {
            double value = vectorFromTxtFile._vector[i++];
            _vec2D.at(x).at(y) = value;
        }
    }
}

void Viewer_Processor::createVec2DFromCLOG()
{

}

void Viewer_Processor::resetDataToDefault()
{
    _vec2D.clear();
    _vec2DMask.clear();
    _fileName.clear();
    _frames.clear();
    _rows = 0;
    _columns = 0;
    _fileType = fileType::UNDEFINED;
    _markers = NO_MARKERS;
}

double Viewer_Processor::findMaxInArrayOrigin()
{
    double max = 0;
    double value = 0;

    for (size_t y = 0; y < _rows; ++y)
    {
        for (size_t x = 0; x < _columns; ++x)
        {
            value = _vec2D[x][y];
            if(max < value)
                max = value;
        }
    }
    return max;
}

double Viewer_Processor::findMinInArrayOrigin()
{
    double min = std::numeric_limits<double>::max();
    double value = 0;

    for (size_t y = 0; y < _rows; ++y)
        for (size_t x = 0; x < _columns; ++x) {
            value = _vec2D[x][y];
            if(min > value) min = value;
        }

    return min;
}

double Viewer_Processor::convert(double value, double From1, double From2, double To1, double To2)
{
    if( (static_cast<int>(From1) == static_cast<int>(From2)) ||  (static_cast<int>(To1) == static_cast<int>(To2)))
        return 0.0;
    return (value-From1)/(From2-From1)*(To2-To1)+To1;
}

void Viewer_Processor::rebuildVec2DAccordingToSettings()
{
    if(!checkSettingsPtr())
        return;
    if(_settings->value("SettingsImage/FrameGroupBox").toBool())
    {
        createFrameInVec2D();
        _markers |= BORDER;
    }
    else
        _markers &= ~BORDER;
    if(_settings->value("SettingsImage/MasquradingGroupBox").toBool())
    {
        createMaskInVec2D();
        _markers |= MASKING;
    }
    else
        _markers &= ~MASKING;

    // CLOG calibration
}

void Viewer_Processor::rebuildImageAccordingToSettings(QImage &image)
{
    if(!checkSettingsPtr())
        return;

    if(_settings->value("SettingsImage/MasquradingGroupBox").toBool())
    {
    //рисуем маскированые пиксели выбраным цветом
    for (size_t  x = 0; x < _columns; ++x)
        for (size_t  y = 0; y < _rows; ++y)
            if(_vec2DMask.at(x).at(y) > 0)
                image.setPixelColor(static_cast<int>(x), static_cast<int>(y), QColor(_settings->value("SettingsImage/maskColor").toString()));
    }
}

void Viewer_Processor::createFrameInVec2D()
{
    if(!checkSettingsPtr())
        return;
    size_t  width = static_cast<size_t>(_settings->value("SettingsImage/frameWidth").toInt());
    if(width > _columns || width > _rows)
        width = 0;
//        width = (_columns > _rows) ? _rows : _columns;
    int value = _settings->value("SettingsImage/frameValue", -1).toInt();
    //верх
    for (size_t  x = 0; x < _columns; ++x)
        for (size_t  y = 0; y < width; ++y)
            _vec2D.at(x).at(y) = value;
    //низ
    for (size_t  x = 0; x < _columns; ++x)
        for (size_t  y = _rows - 1; y >= _rows - width; --y)
            _vec2D.at(x).at(y) = value;
    //лево
    for (size_t  y = 0; y < _rows; ++y)
        for (size_t  x = 0; x < width; ++x)
            _vec2D.at(x).at(y) = value;
    //право
    for (size_t  y = 0; y < _rows; ++y)
        for (size_t  x = _columns - 1; x >= _columns - width; --x)
            _vec2D.at(x).at(y) = value;
}

void Viewer_Processor::createMaskInVec2D()
{
    if(!checkSettingsPtr())
        return;
    allocateEmptyVec2D(_vec2DMask, _columns, _rows);
    int value = _settings->value("SettingsImage/maskValue").toInt();
    int newValue = _settings->value("SettingsImage/maskNewValue").toInt();
    bool after= _settings->value("SettingsImage/maskAfter").toBool();

    for (size_t  x = 0; x < _columns; ++x)
    {
        for (size_t  y = 0; y < _rows; ++y)
        {
            if(after)
            {
                if(_vec2D.at(x).at(y) > value)
                {
                    _vec2D.at(x).at(y) = newValue;
                    _vec2DMask.at(x).at(y) = 1;
                }
            }
            else if(_vec2D.at(x).at(y) < value)
            {
                _vec2D.at(x).at(y) = newValue;
                _vec2DMask.at(x).at(y) = 1;
            }
        }
    }
}
