#ifndef VIEWER_WIDGET_H
#define VIEWER_WIDGET_H

#include <QWidget>
#include <QImage>
#include <QSettings>
#include <QSplitter>

#include "../eventfilter/fingerslide.h"
#include "frames/frames.h"
#include "viewer/viewer.h"
#include "masksettings.h"

namespace Ui {
class Viewer_widget;
}

class Viewer_widget : public QWidget
{
    Q_OBJECT

public:
    explicit Viewer_widget(std::shared_ptr<const QSettings> spSetting, QWidget *parent = nullptr);
    ~Viewer_widget();

    std::shared_ptr<const QSettings>_spSettings;
    std::pair<const Frames &, bool> getFrames() const;

    void setImageFile(const QString &path);

private:
    void makeMaskTab();

    enum tabsName {VIEW_AND_EDIT, MASK};

    QString processing_arrays(const std::vector<std::vector<double>> &origin_array,
                              const std::vector<std::vector<double>> &mask_array);

    Ui::Viewer_widget *ui;

    QSplitter _main_splitter, _left_splitter, _right_splitter;
    Viewer _graphicsView_origin, _mask_viewer, _graphicsView_Result;
    MaskSettings _mask_settings;

private slots:
    void slotTabChanged(int);
    void slotReconstruct_deconv();
};

#endif // VIEWER_WIDGET_H
