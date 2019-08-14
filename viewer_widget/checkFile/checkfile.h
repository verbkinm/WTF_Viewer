#ifndef CHECKFILE_H
#define CHECKFILE_H

#include <QVector>

struct VectorFromTxtFile
{
    size_t  _row = 0;
    size_t  _column = 0;
    QVector<double> _vector;

    VectorFromTxtFile(const QString &fileName);

    QVector<double> checkFile(const QString &fileName);
};



#endif // CHECKFILE_H
