#ifndef PIXELCALIBRATION_H
#define PIXELCALIBRATION_H

#include <QWidget>

namespace Ui {
class PixelCalibration;
}

class PixelCalibration : public QWidget
{
    Q_OBJECT

public:
    explicit PixelCalibration(QWidget *parent = nullptr);
    ~PixelCalibration();

private:
    Ui::PixelCalibration *ui;
};

#endif // PIXELCALIBRATION_H
