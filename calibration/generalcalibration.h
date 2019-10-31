#ifndef GENERALCALIBRATION_H
#define GENERALCALIBRATION_H

#include <QDialog>
#include <QSettings>
#include <memory>

namespace Ui {
class GeneralCalibration;
}

class GeneralCalibration : public QDialog
{
    Q_OBJECT

public:
    explicit GeneralCalibration(std::shared_ptr<QSettings> settings, QWidget *parent = nullptr);
    ~GeneralCalibration();

    double getA();
    double getB();
    double getC();
    double getT();

    bool   getApply();

    void writeSettings();

private:
    Ui::GeneralCalibration *ui;
    std::shared_ptr<QSettings> pSettings;

    void    readSettings();
};

#endif // GENERALCALIBRATION_H
