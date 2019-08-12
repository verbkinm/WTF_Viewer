#ifndef VIEWER_WIDGET_H
#define VIEWER_WIDGET_H

#include <QWidget>
#include <QFile>
#include <QImage>
#include <QCloseEvent>
#include <QGraphicsScene>
#include <QSettings>
#include <QSplitter>

#include "../eventfilter/fingerslide.h"
#include "frames/frames.h"
#include "viewer.h"
#include "masksettings.h"

namespace Ui {
class Viewer_widget;
}

class Viewer_widget : public QWidget
{
    Q_OBJECT

public:

    explicit Viewer_widget(QSettings& setting, QWidget *parent = nullptr);
    ~Viewer_widget();

    QSettings*   _pSettings;

    void        setImageFile(QString path);
    Frames*     getFrames();
    QImage      getImageFromTxtFile(QString file);

private:
    Ui::Viewer_widget *ui;

    QSplitter main_splitter, left_splitter, right_splitter;

    Viewer graphicsView_origin, mask_viewer, graphicsView_Result;
    MaskSettings mask_settings;

    void makeMaskTab();

private slots:
    void slotTabChanged(int);
};

#endif // VIEWER_WIDGET_H
