#include "checkfile.h"
#include <QFile>
#include <QTemporaryFile>
#include <QDebug>

VectorFromTxtFile::VectorFromTxtFile(const QString &fileName)
{
     this->checkFile(fileName);
}
QVector<double> VectorFromTxtFile::checkFile(const QString &fileName)
{
    QFile originFile(fileName);
    originFile.open(QIODevice::ReadOnly);

    QTemporaryFile file;

    if(file.open() )
    {
        file.write(originFile.readAll());

        char c;
        //замена символов табуляции, если они есть, на пробелы
        while (!file.atEnd()){
            file.getChar(&c);
            if(c == '\t'){
                qint64 pos = file.pos();
                file.seek(--pos);
                file.putChar(' ');
            }
        }

        file.seek(0);

        //переменная для наибольшего кол-ва элементов в строке
        int maxCountElement = 0;
        int countString  = 0;

        //определяем наибольшее кол-во элементов в строке
        while (!file.atEnd()) {
            int counInLine = QString(file.readLine()).remove("\r\n").split(" ").length();
            maxCountElement < counInLine ? maxCountElement = counInLine: 0 ;
            countString++;
        }
        file.seek(0);

        while (!file.atEnd()) {
            QStringList splitString = QString(file.readLine()).remove("\r\n").split(" ");
            int countInLine = splitString.length();

            foreach (QString element, splitString)
                _vector.append(element.toDouble());

            //если в текущей строке элементов меньше чем в максимальной строке - добавляем нули
            while (int(countInLine )!= maxCountElement)
            {
                _vector.append(0);
                countInLine++;
            }

        }
        _column = size_t(maxCountElement);
        _row    = size_t(countString);
        file.close();
    }
    else {
        qDebug() << "file " << file.fileName() << " can't open";
    }

    originFile.close();

    return _vector;
}
