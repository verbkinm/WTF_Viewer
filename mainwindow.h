#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QFileSystemModel>
#include <QTreeView>
#include <QMenu>
#include <QSettings>
#include <map>

#include "viewer_widget\viewer_widget.h"
#include "eventfilter\eventfilter.h"
#include "graph/centralwidget.h"
#include "settings/settingsimage.h"
#include "settings/mergetoclog.h"
#include "graph/graphdialog.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void createMenu();
    void openLastDir();
    void saveAccordingOptions(int options, int &error, int &correct, QImage &image, const QString &fullName);
    void exportingFiles(const QString &path);
    void mergingFiles(const QString &path);
    void graphDialogExec(GraphDialog &graphDialog, const Frames &frames);
    std::map<float, float> createVectorAccordingGraphType(GraphDialog &graphDialog, QString &legendText, const Frames &frames);

    std::shared_ptr<QSettings> settings;
    QSplitter _splitter;
    QFileSystemModel _fs_model;
    QTreeView _treeView;
    Viewer_widget _viewerWidget;
    EventFilter _eventFilter;
    const QString _programVersion;
    QMenu _menuFile, _menuGraph, _menuAbout, _menuSettings, _menuCalibration;
    std::list<CentralWidget *> _graphWindowMap;
    QFileInfo _currentFile;

private slots:
    void slotSelectFile(const QModelIndex &index);
    void slotAuthor();
    void slotPlotGraph();
    void slotGeneralCalibration();
    void slotPixelCalibration();
    void slotSettingsImage();
    void slotSettingsOpenClogFile();
    void slotExportFiles();
    void slotBatchProcessing();
    void slotMergeToClog();
    void slotCloseGraphWindow(QObject *obj);
    //при выборе типа данных для диаграммы по оси X, проверяем чтобы не было попытки добавить
    //новый график с одним типом к, существующим графикам с другим типом
    void slotGrapgWindowCheck(const QString &);
};

#endif // MAINWINDOW_H
