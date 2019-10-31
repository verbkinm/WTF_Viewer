#ifndef GRAPHDIALOG_H
#define GRAPHDIALOG_H

#include <QDialog>
#include <memory>
#include <QSettings>
#include "../viewer_widget/frames/frames.h"

namespace Ui {
class GraphDialog;
}

class GraphDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GraphDialog(std::shared_ptr<QSettings> settings, const Frames& frames, QWidget *parent = nullptr);
    ~GraphDialog();

    size_t getCurrentClusterLenght();
    QString getType();
    QString getClusterSize();
    QString getCurrentWindowGraph();

    void selectLastWindow();

    void clearWindow(); //очистить windowGraph
    void appendWindow(QString); //добавить к windowGraph строку

    const QString _NEW_WINDOW;

private:
    Ui::GraphDialog *ui;
    std::shared_ptr<QSettings> pSettings;

private slots:
    void slotSelectDataX(QString);

signals:
    void signalDataXChanged(QString);
};

#endif // GRAPHDIALOG_H
