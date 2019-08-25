#include "export.h"
#include "ui_export.h"

#include <QFileDialog>

Export::Export(QString path, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Export)
{
    ui->setupUi(this);

    this->path = path;
    ui->ExportTo->setText(path);

    connect(ui->getDir, &QAbstractButton::clicked, this, &Export::slotDirDialog);
    connect(ui->gettFiles, &QAbstractButton::clicked, this, &Export::slotFileDialog);
}
QString Export::getPath()
{
    return ui->ExportTo->text();
}
QStringList Export::getFileNames()
{
    QStringList list = ui->filesToExport->text().split(";");
    list.removeLast();
    return list;
}
int Export::getOption()
{
    if(ui->BW->isChecked())
        return BW;
    else if (ui->WB->isChecked())
        return WB;
    else if(ui->BW_AND_WB->isChecked())
        return BW_AND_WB;

    return  -1;
}
void Export::slotDirDialog()
{
    QString savePath = QFileDialog::getExistingDirectory(this,
                                                         "Selecting a save location",
                                                         path,
                                                         QFileDialog::ShowDirsOnly);
    ui->ExportTo->setText(savePath);
}
void Export::slotFileDialog()
{
    QStringList listFiles = QFileDialog::getOpenFileNames(this,
                                                          "Export Files",
                                                          path,
                                                          "TXT (*.txt)");
    for (auto &str : listFiles)
        ui->filesToExport->setText(ui->filesToExport->text() + str + ";");
}
Export::~Export()
{
    delete ui;
}
