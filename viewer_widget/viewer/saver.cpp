#include <QApplication>
#include <QTextStream>
#include <QDebug>
#include <QMessageBox>
#include <QDir>
#include <QTemporaryFile>
#include <QFileDialog>
#include <QImage>
#include "saver.h"

Saver::Saver(QObject *parent) : QObject(parent)
{

}

void Saver::dialogSaveTXT(size_t columns, size_t rows, const std::vector<std::vector<double>> &vector)
{
    QString fileName = QFileDialog::getSaveFileName(nullptr,
                                                    ("Save file"),
                                                    QDir::rootPath(),
                                                    "TXT (*.txt);;All files (*.*)");
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "file " << file.fileName() << " can't open";
        return;
    }
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QTextStream writeStreame(&file);
    for (size_t y = 0; y < rows; ++y)
    {
        for (size_t x = 0; x < columns; ++x)
        {
            if(x != 0)
                writeStreame << " ";
            writeStreame << QString::number(vector.at(x).at(y), 'f', 2);
        }
        writeStreame << "\n";
        writeStreame.flush();
    }
    file.close();
    QApplication::restoreOverrideCursor();
    QMessageBox::information(nullptr, "Saved", "File \"" + fileName + "\" successfully saved!");
}

QString Saver::saveInTemporaryTXT(size_t columns, size_t rows, const std::vector<std::vector<double>> &vector)
{
    QTemporaryFile tmpFile;
    tmpFile.setAutoRemove(false);
    tmpFile.open();
    tmpFile.close();

    QFile file(tmpFile.fileName() + ".txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "file " << file.fileName() << " can't open";
        return nullptr;
    }
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QTextStream writeStreame(&file);
    for (size_t y = 0; y < rows; ++y)
    {
        for (size_t x = 0; x < columns; ++x)
        {
            if(x != 0)
                writeStreame << " ";
            writeStreame << QString::number(vector.at(x).at(y), 'f', 2);
        }
        writeStreame << "\n";
        writeStreame.flush();
    }
    file.close();
    QApplication::restoreOverrideCursor();

    return file.fileName();
}

void Saver::dialogSaveBMP(const QImage &image)
{
    QString fileName = QFileDialog::getSaveFileName(nullptr,
                                                    ("Save file"),
                                                    QDir::rootPath(),
                                                    "Images (*.bmp);;All files (*.*)");

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    image.save(fileName, "BMP");
    QApplication::restoreOverrideCursor();
    QMessageBox::information(nullptr, "Saved", "File \"" + fileName + "\" successfully saved!");
}
