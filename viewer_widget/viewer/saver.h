#ifndef SAVER_H
#define SAVER_H

#include <QFile>

class Saver : public QObject
{
    Q_OBJECT
public:
    explicit Saver(QObject *parent = nullptr);

    static void saveTXT(size_t columns, size_t rows, const std::vector<std::vector<double>>&);
    static QString temporaryTXT(size_t columns, size_t rows, const std::vector<std::vector<double>>&);
    static void saveBMP(const QImage &image);

signals:

public slots:
};

#endif // SAVER_H
