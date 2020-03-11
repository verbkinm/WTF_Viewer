#ifndef SAVER_H
#define SAVER_H

#include <QFile>

class Saver : public QObject
{
public:
    explicit Saver(QObject *parent = nullptr);

    static void dialogSaveTXT(size_t columns, size_t rows, const std::vector<std::vector<float> > &);
    static void dialogSaveBMP(const QImage &image);

    static QString saveInTemporaryTXT(size_t columns, size_t rows, const std::vector<std::vector<float> > &);
};

#endif // SAVER_H
