#ifndef MERGETOCLOG_H
#define MERGETOCLOG_H

#include <QDialog>

namespace Ui {
class MergeToCLog;
}

class MergeToCLog : public QDialog
{
    Q_OBJECT

public:
    explicit MergeToCLog(QString _path, QWidget *parent = nullptr);
    ~MergeToCLog();

    QString getPath();
    QStringList getFileNames();

private:
    Ui::MergeToCLog *ui;
    QString _path;

private slots:
    void slotDirDialog();
    void slotFileDialog();
};

#endif // MERGETOCLOG_H
