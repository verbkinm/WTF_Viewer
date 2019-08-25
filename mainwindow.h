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
#include "graph/graphdialog.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    std::shared_ptr<QSettings> settings;
    QSplitter _splitter;
    QFileSystemModel _fs_model;
    QTreeView _treeView;
    Viewer_widget _viewerWidget;
    EventFilter _eventFilter;
    const QString _programVersion;
    QMenu _menuFile, _menuGraph, _menuAbout, _menuSettings, _menuCalibration;
    std::map<QString, CentralWidget *> _graphWindowMap;
    QString _currentActiveFile;

    void createMenu();
    void openLastDir();
    void saveAccordingOptions(int, int &, int &, QImage &, const QString &);
    void exportingFiles(const QString &);
    void graphDialogExec(GraphDialog &, const Frames &);
    std::vector<QPointF> createVectorAccordingGraphType(GraphDialog &, QString &, const Frames &);

private slots:
    void slotSelectFile(const QModelIndex &);
    void slotAuthor();
    void slotPlotGraph();
    void slotGeneralCalibration();
    void slotPixelCalibration();
    void slotSettingsImage();
    void slotExportFiles();
    void slotCloseGraphWindow(QObject *);
    //при выборе типа данных для диаграммы по оси X, проверяем чтобы не было попытки добавить
    //новый график с одним типом к, существующим графикам с другим типом
    void slotGrapgWindowCheck(const QString &);
};

#endif // MAINWINDOW_H
