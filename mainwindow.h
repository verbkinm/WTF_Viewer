#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QFileSystemModel>
#include <QTreeView>
#include <QStatusBar>
#include <QMenuBar>
#include <QMenu>
#include <QEvent>
#include <QTreeWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QSettings>

#include "viewer\viewer_widget.h"
#include "eventfilter\eventfilter.h"
#include "graph/centralwidget.h"
#include "settings/settingsimage.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QSplitter*          pSplitter       = nullptr;
    QFileSystemModel*   pFSModel        = nullptr;
    QTreeView*          pTreeView       = nullptr;

    Viewer_widget*      pViewerWidget   = nullptr;
    EventFilter*        pEventFilter    = nullptr;

    SettingsImage*      pSettingsImage  = nullptr;

private:

    QMenu*              pMenuFile       = nullptr;
    QMenu*              pMenuGraph      = nullptr;
    QMenu*              pMenuAbout      = nullptr;
    QMenu*              pMenuSettings   = nullptr;
    QMenu*              pMenuCalibration= nullptr;


    QList<CentralWidget*> graphWindowList;

    void                createMenu();

    QString             currentActiveFile;

    QSettings           settings;

protected:
    virtual bool        event(QEvent *event);

private slots:
    void slotSelectFile(const QModelIndex&);

    void slotAuthor();
    void slotPlotGraph();
    void slotGeneralCalibration();
    void slotPixelCalibration();

    void slotSettingsImage();

    void slotExportFile();

    void slotCloseGraphWindow(QObject* obj);

    //при выборе типа данных для диаграммы по оси X, проверяем чтобы не было попытки добавить
    //новый график с одним типом к, существующим графикам с другим типом
    void slotGrapgWindowCheck(QString value);

    // QWidget interface
protected:
    virtual void closeEvent(QCloseEvent *);
};

#endif // MAINWINDOW_H
