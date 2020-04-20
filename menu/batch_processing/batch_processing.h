#ifndef BATCH_PROCESSING_H
#define BATCH_PROCESSING_H

#include <QDialog>
#include <QFileInfo>
#include "../../viewer_widget/frames/frames.h"

namespace Ui {
class Batch_Processing;
}

class Batch_Processing : public QDialog
{
    Q_OBJECT

public:
    explicit Batch_Processing(const QFileInfo &file, const Frames &frames, QWidget *parent = nullptr);
    ~Batch_Processing();

    void slice();

private:
    Ui::Batch_Processing *ui;
    const Frames &_frames;
    const QFileInfo &_file;

private slots:
    void slotOpenClogFilesDistination();
    void slotOpenTiffFilesDistination();

    void slotNumberOfPacketsChanged(int value);
    void slotWorkSpacePacketsChanged(int left, int center, int right);
};

#endif // BATCH_PROCESSING_H
