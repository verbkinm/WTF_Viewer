#include <QFileDialog>
#include <QDir>

#include "batch_processing.h"
#include "ui_batch_processing.h"
#include "batch_processing_cutter.h"

Batch_Processing::Batch_Processing(const QFileInfo &file, const Frames &frames, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Batch_Processing),
    _frames(frames),
    _file(file)
{
    ui->setupUi(this);

    ui->trimming_begin_and_and_of_a_file->setTitle("Initial empty frames", "Work space (frames)", "Final empty frames");
    ui->good_frames->setTitle("First frames of packets", "Good frames", "Last frames of packets");
    ui->clog_location->setText(file.filePath());

    connect(ui->getClogDistinationFiles, SIGNAL(clicked()), SLOT(slotOpenClogFilesDistination()));
    connect(ui->getTiffDistinationFiles, SIGNAL(clicked()), SLOT(slotOpenTiffFilesDistination()));

    ui->trimming_begin_and_and_of_a_file->setMaximum(_frames.getFrameCount());
    ui->good_frames->setMaximum(_frames.getFrameCount());
    ui->numberOfPackets->setMaximum(_frames.getFrameCount());

    connect(ui->numberOfPackets, SIGNAL(valueChanged(int)), SLOT(slotNumberOfPacketsChanged(int)));
    connect(ui->trimming_begin_and_and_of_a_file, SIGNAL(signalValueChanged(int, int, int)), SLOT(slotWorkSpacePacketsChanged(int, int, int)));
}

Batch_Processing::~Batch_Processing()
{
    delete ui;
}

void Batch_Processing::slice()
{
    Batch_Processing_Cutter cutter(_frames);
    cutter.setData(ui->trimming_begin_and_and_of_a_file->getLeftValue(), ui->trimming_begin_and_and_of_a_file->getCenterValue(), ui->trimming_begin_and_and_of_a_file->getRightValue(),
                   ui->numberOfPackets->value(),
                   ui->good_frames->getLeftValue(), ui->good_frames->getCenterValue(), ui->good_frames->getRightValue());
    cutter.slice(ui->exportClogFilesDistination->text());
}

void Batch_Processing::slotOpenClogFilesDistination()
{
    QString savePath = QFileDialog::getExistingDirectory(this,
                                                         "Selecting a save location",
                                                         QDir::rootPath(),
                                                         QFileDialog::ShowDirsOnly);
    ui->exportClogFilesDistination->setText(savePath);

    if(savePath.isEmpty())
        ui->buttonBox->setEnabled(false);
    else
        ui->buttonBox->setEnabled(true);
}

void Batch_Processing::slotOpenTiffFilesDistination()
{
    QString savePath = QFileDialog::getExistingDirectory(this,
                                                         "Selecting a save location",
                                                         QDir::rootPath(),
                                                         QFileDialog::ShowDirsOnly);
    ui->exportTifFilesDistination->setText(savePath);
}

void Batch_Processing::slotNumberOfPacketsChanged(int value)
{
    ui->good_frames->setMaximum(ui->trimming_begin_and_and_of_a_file->getCenterValue() / value);
}

void Batch_Processing::slotWorkSpacePacketsChanged(int left, int center, int right)
{
    Q_UNUSED(left);
    Q_UNUSED(right);

    ui->good_frames->setMaximum(center / ui->numberOfPackets->value());
}
