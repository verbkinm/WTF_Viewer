#ifndef PREFILTER_H
#define PREFILTER_H

#include <QDialog>
#include <QFileInfo>

#include "frames/frames.h"

namespace Ui {
class PreFilter;
}

class PreFilter : public QDialog
{
    Q_OBJECT

public:
    explicit PreFilter(const QString &fileName, const Frames &frames, QWidget *parent = nullptr);
    ~PreFilter();

    size_t getFrameMin() const;
    size_t getFrameMax() const;

private:
    QString getSize(const QFileInfo &fi);

    Ui::PreFilter *ui;

};

#endif // PREFILTER_H
