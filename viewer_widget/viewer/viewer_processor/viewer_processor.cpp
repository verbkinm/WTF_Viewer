#include <QFileInfo>
#include <QImage>
#include <QDebug>
#include <QtMath>

#include "viewer_processor.h"

Viewer_Processor::Viewer_Processor() :
    _fileType(fileType::UNDEFINED),
    _pSettings(nullptr),
    _rows(0), _columns(0)
{

}

Viewer_Processor::~Viewer_Processor()
{

}

QString Viewer_Processor::getFileName() const
{
    return _fileName;
}

size_t Viewer_Processor::getColumns() const
{
    return _columns;
}

size_t Viewer_Processor::getRows() const
{
    return _rows;
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

const std::vector<std::vector<double> > &Viewer_Processor::getVec2D() const
{
    return _vec2D;
}

void Viewer_Processor::setFileName(const QString &fileName)
{
    resetDataToDefault();
    _fileName = fileName;
    setFileType(fileName);
    createVec2D();
}

void Viewer_Processor::setSettings(std::shared_ptr<QSettings>pSharedSettings)
{
    _pSettings = pSharedSettings;
}

void Viewer_Processor::setDataInVec2D(size_t column_number, size_t row_number, double value)
{
    try
    {
        _vec2D.at(column_number).at(row_number) = value;
    }
    catch(std::out_of_range&)
    {
        return;
    }
}
std::vector<std::vector<double>> Viewer_Processor::cutVec2D(size_t cutX, size_t fromRow, size_t width, size_t height)
{
    std::vector<std::vector<double>> newVec2D(width, std::vector<double>(height));

    double value = 0;
    for (size_t x = cutX, tmpX = 0; tmpX < width; ++x, ++tmpX)
    {
        for (size_t y = fromRow, tmpY = 0; tmpY < height; ++y, ++tmpY) {
            if( (x >= _columns) || (y >= _rows) )
                value = 0;
            else
                value = _vec2D.at(static_cast<size_t>(x)).at(static_cast<size_t>(y));
            newVec2D.at(static_cast<size_t>(tmpX)).at(static_cast<size_t>(tmpY)) = value;
        }
    }
    return newVec2D;
}

Viewer_Processor::fileType Viewer_Processor::getFileType() const
{
    return _fileType;
}

QImage Viewer_Processor::getImageFromVec2D()
{
    double max = findMaxInVec2D();
    double min = findMinInVec2D();

    QImage image(static_cast<int>(_columns), static_cast<int>(_rows), QImage::Format_ARGB32_Premultiplied);
    for (size_t  x = 0; x < _columns; ++x)
    {
        for (size_t  y = 0; y < _rows; ++y)
        {
            double value = rangeConverter(_vec2D.at(x).at(y), \
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
    if(_pSettings != nullptr)
        return true;

    qDebug() << __FUNCTION__ << " _settings = nullptr";
    return false;
}

void Viewer_Processor::rebuildVec2DAccordingToSettings()
{
    if(!checkSettingsPtr())
        return;
    if(_pSettings->value("SettingsImage/FrameGroupBox").toBool())
    {
        createFrameInVec2D();
        _markers |= BORDER;
    }
    else
        _markers &= ~BORDER;
    if(_pSettings->value("SettingsImage/MasquradingGroupBox").toBool())
    {
        createMaskInVec2D();
        _markers |= MASKING;
    }
    else
        _markers &= ~MASKING;
}

void Viewer_Processor::rebuildImageAccordingToSettings(QImage &image)
{
    if(!checkSettingsPtr())
        return;
    if(_pSettings->value("SettingsImage/MasquradingGroupBox").toBool())
    {
    //рисуем маскированые пиксели выбраным цветом
    for (size_t  x = 0; x < _columns; ++x)
        for (size_t  y = 0; y < _rows; ++y)
            if(_vec2DMask.at(x).at(y) > 0)
                image.setPixelColor(static_cast<int>(x), static_cast<int>(y), QColor(_pSettings->value("SettingsImage/maskColor").toString()));
    }
}

void Viewer_Processor::setFileType(const QString &fileName)
{
    QFileInfo file(fileName);
    if(file.suffix() == "txt")
        _fileType = fileType::TXT;
    else if(file.suffix() == "clog")
        _fileType = fileType::CLOG;
    else
        _fileType = fileType::UNDEFINED;
}

void Viewer_Processor::allocateEmptyVec2D(std::vector<std::vector<double>> &vec2D, size_t columns, size_t rows)
{
    vec2D.clear();
    vec2D.resize(columns, std::vector<double>(rows, 0));
}

double Viewer_Processor::findMaxInVec2D()
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

double Viewer_Processor::findMinInVec2D()
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

double Viewer_Processor::rangeConverter(double value, double From1, double From2, double To1, double To2)
{
    if( (static_cast<int>(From1) == static_cast<int>(From2)) ||  (static_cast<int>(To1) == static_cast<int>(To2)))
        return 0.0;
    return (value-From1)/(From2-From1)*(To2-To1)+To1;
}

void Viewer_Processor::createFrameInVec2D()
{
    if(!checkSettingsPtr())
        return;
    size_t  width = static_cast<size_t>(_pSettings->value("SettingsImage/frameWidth").toInt());
    if(width > _columns || width > _rows)
        width = 0;
//        width = (_columns > _rows) ? _rows : _columns;
    int value = _pSettings->value("SettingsImage/frameValue", -1).toInt();
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
    int value = _pSettings->value("SettingsImage/maskValue").toInt();
    int newValue = _pSettings->value("SettingsImage/maskNewValue").toInt();
    bool after= _pSettings->value("SettingsImage/maskAfter").toBool();

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
