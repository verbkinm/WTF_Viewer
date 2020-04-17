#include "mergetoclog.h"
#include "ui_mergetoclog.h"

#include <QFileDialog>

MergeToCLog::MergeToCLog(QString path, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MergeToCLog), path(path)
{
    ui->setupUi(this);
    ui->ExportTo->setText(path);

    connect(ui->getDir, SIGNAL(clicked()), this, SLOT(slotDirDialog()));
    connect(ui->getFiles, SIGNAL(clicked()), this, SLOT(slotFileDialog()));
}

MergeToCLog::~MergeToCLog()
{
    delete ui;
}

QString MergeToCLog::getPath()
{
    return ui->ExportTo->text();
}

QStringList MergeToCLog::getFileNames()
{
    QStringList list = ui->filesToExport->text().split(";");
    list.removeLast();
    return list;
}

void MergeToCLog::slotDirDialog()
{
    QString savePath = QFileDialog::getExistingDirectory(this,
                                                         "Selecting a save location",
                                                         path,
                                                         QFileDialog::ShowDirsOnly);
    if(!savePath.isEmpty())
        ui->ExportTo->setText(savePath);
}

void MergeToCLog::slotFileDialog()
{
    QStringList listFiles = QFileDialog::getOpenFileNames(this,
                                                          "Merge from",
                                                          path,
                                                          "TXT (*.txt)");
    for (auto &str : listFiles)
        ui->filesToExport->setText(ui->filesToExport->text() + str + ";");
}
