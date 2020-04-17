#ifndef EXPORT_H
#define EXPORT_H

#include <QDialog>

namespace Ui {
class Export;
}

class Export : public QDialog
{
    Q_OBJECT

public:
    explicit Export(QString path, QWidget *parent = nullptr);
    ~Export();

    QString getPath();
    QStringList getFileNames();
    int getOption();

    enum OPTION{BW, WB, BW_AND_WB};

private:
    Ui::Export *ui;
    QString path;

private slots:
    void slotDirDialog();
    void slotFileDialog();
};

#endif // EXPORT_H
