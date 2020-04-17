#include <algorithm>
#include <QDebug>
#include "prefilter.h"
#include "ui_prefilter.h"

PreFilter::PreFilter(const QFile &file, const Frames &frames, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreFilter)
{
    ui->setupUi(this);

    QFileInfo fi(file);

    ui->path->setText(fi.absoluteFilePath());
    ui->size->setText(getSize(fi));

    ui->framesMin->setText(QString::number(frames.getFrameMin()));

    ui->framesMax->setText(QString::number(frames.getFrameMax()));
    ui->filterFramesMax->setValue(frames.getFrameMax());
    ui->filterFramesMax->setMinimum(frames.getFrameMin());
    ui->filterFramesMax->setMaximum(frames.getFrameMax());

    ui->filterFramesMin->setValue(frames.getFrameMin());
    ui->filterFramesMin->setMinimum(frames.getFrameMin());
    ui->filterFramesMin->setMaximum(frames.getFrameMax());

    ui->clustersMin->setText(QString::number(frames.getClusterMin()));
    ui->clustersMax->setText(QString::number(frames.getClusterMax()));

    ui->totsMin->setText(QString::number(frames.getTotMin()));
    ui->totsMax->setText(QString::number(frames.getTotMax()));
}

PreFilter::~PreFilter()
{
    delete ui;
}

size_t PreFilter::getFrameMin() const
{
    return ui->filterFramesMin->value();
}

size_t PreFilter::getFrameMax() const
{
    return ui->filterFramesMax->text().toULongLong();
}

QString PreFilter::getSize(const QFileInfo &fi)
{
    quint64 size = fi.size();
    QString result = "0 byte";

    if(size > 0 && size < 1024)
            result = QString::number(size) + " B";
    else if(size > (1024 * 1024 * 1024))
        result = QString::number(static_cast<double>(size) / 1024 / 1024 / 1024) + " GB";
    else if(size > (1024 * 1024))
        result = QString::number(static_cast<double>(size) / 1024 /1024) + " MB";
    else if(size > 1024)
        result = QString::number(static_cast<double>(size) / 1024) + " KB";

    return  result;
}
