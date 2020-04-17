#include <QTemporaryFile>
#include <QDebug>

#include "checkfile.h"

VectorFromTxtFile::VectorFromTxtFile(const QString &fileName) :
    _row(0), _column(0),
    _vector(0),
    _maxCountElementString(0), _countStringsFile(0),
    _countInLine(0)
{
    this->createVectorFromTxtFile(fileName);
}
std::vector<float> VectorFromTxtFile::createVectorFromTxtFile(const QString &fileName)
{
    QFile originFile(fileName);
    if(!originFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "file " << originFile.fileName() << " can't open";
        return _vector;
    }
    if(!_file.open())
    {
        qDebug() << "file " << _file.fileName() << " can't open";
        return _vector;
    }
    _file.write(originFile.readAll());
    originFile.close();
    tabToSpace();
    setMaxCountElementsInString();
    createDataVector();
    _column = _maxCountElementString;
    _row = _countStringsFile;
    _file.close();
    return _vector;
}

void VectorFromTxtFile::tabToSpace()
{
    char c;
    //замена символов табуляции, если они есть, на пробелы
    _file.seek(0);
    while (!_file.atEnd())
    {
        _file.getChar(&c);
        if(c == '\t')
        {
            qint64 pos = _file.pos();
            _file.seek(--pos);
            _file.putChar(' ');
        }
    }
    _file.seek(0);
}

void VectorFromTxtFile::setMaxCountElementsInString()
{
    //определяем наибольшее кол-во элементов в строке и счетаем кол-во строк
    while (!_file.atEnd())
    {
        size_t counInLine = static_cast<size_t>(QString(_file.readLine()).remove("\r\n").split(" ").length());
        if(_maxCountElementString < counInLine)
            _maxCountElementString = counInLine;
        _countStringsFile++;
    }
    _file.seek(0);
}

void VectorFromTxtFile::createDataVector()
{
    while (!_file.atEnd())
    {
        QStringList splitString = QString(_file.readLine()).remove("\r\n").split(" ");
        _countInLine = static_cast<size_t>(splitString.length());

        for (auto element : splitString)
            _vector.push_back(element.toFloat());
        //если в текущей строке элементов меньше чем в максимальной строке - добавляем нули
        while (_countInLine != _maxCountElementString)
        {
            _vector.push_back(0);
            _countInLine++;
        }
    }
}
