#ifndef SETTINGSIMAGE_H
#define SETTINGSIMAGE_H

#include <QDialog>
#include <QSettings>
#include <memory>

namespace Ui {
class SettingsImage;
}

class SettingsImage : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsImage(std::shared_ptr<QSettings> pSettings, QWidget *parent = nullptr);
    ~SettingsImage();

    void writeSettings();

private slots:
    void on_actionAccepted_triggered();
    void on_actionReject_triggered();
    void on_actionsetColor_triggered();

private:
    std::shared_ptr<QSettings> pSettings;
    Ui::SettingsImage *ui;


    void readSettings();
};

#endif // SETTINGSIMAGE_H
