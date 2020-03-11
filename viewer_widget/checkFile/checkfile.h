#ifndef CHECKFILE_H
#define CHECKFILE_H

#include <QVector>
#include <QFile>
#include <QTemporaryFile>

struct VectorFromTxtFile
{
    size_t _row;
    size_t _column;
    std::vector<float> _vector;

    VectorFromTxtFile(const QString &fileName);
    std::vector<float> createVectorFromTxtFile(const QString &fileName);

private:
    size_t _maxCountElementString; //переменная для наибольшего кол-ва элементов в строке
    size_t _countStringsFile; // кол-во строк в файле
    size_t _countInLine;
    QTemporaryFile _file;

    void tabToSpace();
    void setMaxCountElementsInString();
    void createDataVector();
};
#endif // CHECKFILE_H
