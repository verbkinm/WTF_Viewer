#ifndef SETTINGSCLOGFILE_H
#define SETTINGSCLOGFILE_H

#include <QDialog>
#include <QSettings>
#include <memory>

namespace Ui {
class SettingsClogFile;
}

class SettingsClogFile : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsClogFile(std::shared_ptr<QSettings> settings, QWidget *parent = nullptr);
    ~SettingsClogFile();

    void writeSettings();

private:
    std::shared_ptr<QSettings> pSettings;
    Ui::SettingsClogFile *ui;

    void readSettings();
};

#endif // SETTINGSCLOGFILE_H
