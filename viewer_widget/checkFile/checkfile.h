#ifndef CHECKFILE_H
#define CHECKFILE_H

#include <QList>

struct ListData
{
    size_t  row     = 0;
    size_t  column  = 0;
    QList<double>  list;

    ListData(QString fileName);

    QList<double> checkFile(QString fileName);
};



#endif // CHECKFILE_H
