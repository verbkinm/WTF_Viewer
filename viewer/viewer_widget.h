#ifndef VIEWER_WIDGET_H
#define VIEWER_WIDGET_H

#include <QWidget>
#include <QFile>
#include <QImage>
#include <QCloseEvent>
#include <QGraphicsScene>
#include <QSettings>

#include "../eventfilter/fingerslide.h"
#include "frames/frames.h"

namespace Ui {
class Viewer_widget;
}

class Viewer_widget : public QWidget
{
    Q_OBJECT

public:

    explicit Viewer_widget(QSettings& setting, QWidget *parent = nullptr);
    ~Viewer_widget();

    QSettings   _settings;

    void        setImageFile(QString path);
    Frames*     getFrames();
    QImage      getImageFromTxtFile(QString file);

private:
    Ui::Viewer_widget *ui;

private slots:
    void slotTabChanged(int);
};

#endif // VIEWER_WIDGET_H
