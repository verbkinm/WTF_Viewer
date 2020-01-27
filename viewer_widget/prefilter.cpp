#include <algorithm>
#include <QDebug>
#include "prefilter.h"
#include "ui_prefilter.h"

PreFilter::PreFilter(const QString &fileName, const Frames &frames, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreFilter)
{
    ui->setupUi(this);

    QFileInfo fi(fileName);

    ui->path->setText(fi.absoluteFilePath());
    ui->size->setText(getSize(fi));

    ui->framesMax->setText(QString::number(frames.getFrameCount() - 1));
    ui->filterFramesMax->setText(QString::number(frames.getFrameCount() - 1));

    ui->clustersMin->setText(QString::number(frames.getClusterMin()));
    ui->clustersMax->setText(QString::number(frames.getClusterMax()));

    ui->totsMin->setText(QString::number(frames.getTotMin()));
    ui->totsMax->setText(QString::number(frames.getTotMax()));

    connect(ui->filterFramesMin, SIGNAL(textEdited(const QString &)), SLOT(slotCheckDigit(const QString &)));
    connect(ui->filterFramesMax, SIGNAL(textEdited(const QString &)), SLOT(slotCheckDigit(const QString &)));
}

PreFilter::~PreFilter()
{
    delete ui;
}

size_t PreFilter::getFrameMin() const
{
    return ui->filterFramesMin->text().toULongLong() + 1;
}

size_t PreFilter::getFrameMax() const
{
    return ui->filterFramesMax->text().toULongLong() + 1;
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

void PreFilter::slotCheckDigit(const QString &text)
{
    QLineEdit *lineEdit = qobject_cast<QLineEdit*>(sender());
    if(!text.length())
    {
        lineEdit->setText("0");
        return;
    }
    QPalette palette = lineEdit->palette();
    palette.setColor(lineEdit->foregroundRole(), Qt::black);
    for (auto &ch : text)
        if(!ch.isDigit())
            palette.setColor(lineEdit->foregroundRole(), Qt::red);

    lineEdit->setPalette(palette);
    if(palette.color(lineEdit->foregroundRole()) == Qt::red)
        ui->buttonBox->setDisabled(true);
    else
        ui->buttonBox->setDisabled(false);
}
