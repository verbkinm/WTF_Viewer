#ifndef GENERALCALIBRATION_H
#define GENERALCALIBRATION_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class GeneralCalibration;
}

class GeneralCalibration : public QDialog
{
    Q_OBJECT

public:
    explicit GeneralCalibration(QSettings& settings, QWidget *parent = nullptr);
    ~GeneralCalibration();

    double getA();
    double getB();
    double getC();
    double getT();

    bool   getApply();

    void writeSettings();

private:
    Ui::GeneralCalibration *ui;
    QSettings* settings = nullptr;

    void    readSettings();
};

#endif // GENERALCALIBRATION_H
