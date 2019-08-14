#include <QApplication>
#include <QHeaderView>
#include <QDebug>

#include "mainwindow.h"
#include "export\export.h"
#include "graph/graphdialog.h"
#include "calibration/generalcalibration.h"

const static QString VERSION =  "0.9.8.7";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), settings(QSettings::IniFormat, QSettings::UserScope, "WTF.org", "WTF")
{
    settings.setIniCodec("UTF-8");

    createMenu();

    _splitter.setOrientation(Qt::Horizontal);
    pViewerWidget = new Viewer_widget(settings, this);
    _eventFilter.setParent(&_treeView);
    _treeView.installEventFilter(&_eventFilter);

    _fs_model.setRootPath(QDir::rootPath());
    QStringList filter;
    filter << "*.txt" << "*.clog";
    _fs_model.setNameFilters(filter);
    _fs_model.setNameFilterDisables(false);

    _treeView.setModel(&_fs_model);
    _treeView.header()->hideSection(1);
    _treeView.header()->hideSection(2);
    _treeView.header()->hideSection(3);
    _treeView.setAnimated(true);

    _splitter.addWidget(&_treeView);
    _splitter.addWidget(pViewerWidget);

    this->setCentralWidget(&_splitter);

    connect(&_treeView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(slotSelectFile(const QModelIndex&)));
    connect(&_treeView, SIGNAL(activated(const QModelIndex&)), this, SLOT(slotSelectFile(const QModelIndex&)));

    this->setWindowIcon(QIcon(":/atom"));
    this->setWindowTitle("WTF_Viewer " + VERSION);

    openLastDir();
}
void MainWindow::createMenu()
{
    _menuFile.setTitle("File");
    _menuFile.addAction(QIcon(":/save_as"), "Export files", this, SLOT(slotExportFile()));

    _menuFile.addSeparator();

    _menuFile.addAction(QIcon(":/exit"), "Exit", QApplication::instance(), SLOT(quit()));

    _menuSettings.setTitle("Settings");
    _menuSettings.addAction(QIcon(":/image"), "Image", this, SLOT(slotSettingsImage()));

    _menuGraph.setTitle("Graph");
    _menuGraph.addAction(QIcon(":/graph"), "Plot the graph", this, SLOT(slotPlotGraph()));
    _menuGraph.setDisabled(true);

    _menuAbout.setTitle("About");
    _menuAbout.addAction(QIcon(":/author"),"Author", this, SLOT(slotAuthor()));
    _menuAbout.addAction(QIcon(":/qt_logo"), "About Qt", QApplication::instance(), SLOT(aboutQt()));

    _menuCalibration.setTitle("Calibration");
    _menuCalibration.addAction(QIcon(":/"),"General calibration", this, SLOT(slotGeneralCalibration()));
    _menuCalibration.addAction(QIcon(":/"),"Pixel masturbation calibration", this, SLOT(slotPixelCalibration()));


    this->menuBar()->addMenu(&_menuFile);
    this->menuBar()->addMenu(&_menuSettings);
    this->menuBar()->addMenu(&_menuGraph);
    this->menuBar()->addMenu(&_menuCalibration);
    this->menuBar()->addMenu(&_menuAbout);
}

void MainWindow::openLastDir()
{
    QModelIndex lastDirIndex = _fs_model.index(settings.value("Path/lastDir").toString());
    _treeView.expand(lastDirIndex);

    QModelIndex index = lastDirIndex;
    while (index.parent().isValid())
    {
        QModelIndex parent = index.parent();
        _treeView.expand(parent);
        index = parent;
    }
}
void MainWindow::slotExportFile()
{
    QFileInfo file(_fs_model.filePath(_treeView.currentIndex()));
    QString path;
    if(file.isDir())
        path = file.absoluteFilePath();
    else if(file.isFile())
        path = file.absolutePath();

    Export exportWindow(path, this);
    if(exportWindow.exec() == QDialog::Accepted)
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        int correct = 0;
        int error   = 0;
        QStringList listFiles = exportWindow.getFileNames();

        for (auto &fileName : listFiles)
        {
            QImage image(pViewerWidget->getImageFromTxtFile(fileName));
            if(image.format() != QImage::Format_Invalid)
            {
                QFileInfo fileInfo(fileName);
                QFileInfo resultFile(QDir(exportWindow.getPath()), fileInfo.baseName());
                QString fullName = resultFile.absoluteFilePath();
                switch (exportWindow.getOption())
                {
                    case (Export::BW) :
                        if(image.save(fullName+".bmp", "BMP"))
                            correct++;
                        else
                            error++;
                        break;
                    case (Export::WB) :
                        image.invertPixels();
                        if(image.save(fullName+"_INVERSION.bmp", "BMP"))
                            correct++;
                        else
                            error++;
                        break;
                    case (Export::BW_AND_WB) :
                        if(image.save(fullName+".bmp", "BMP"))
                            correct++;
                        image.invertPixels();
                        if(image.save(fullName+"_INVERSION.bmp", "BMP"))
                            correct++;
                        break;
                }
            }
            else
                error++;
        }
        QApplication::restoreOverrideCursor();
        QMessageBox::information(this, "Export", "Export " + QString::number(correct) +
                                 " file(s)<br>Error export " + QString::number(error) + " file(s)");
    }
}

void MainWindow::slotCloseGraphWindow(QObject *obj)
{
    delete obj;
    graphWindowList.removeOne(static_cast<CentralWidget*>(obj));
}

void MainWindow::slotGrapgWindowCheck(QString value)
{
    GraphDialog* gd = static_cast<GraphDialog*>(sender());

    gd->clearWindow();

    for (auto *cw : graphWindowList)
    {
        if(cw->getDataXType() != value)
            continue;
        gd->appendWindow(cw->getTitle());
    }

    gd->selectLastWindow();
}

void MainWindow::closeEvent(QCloseEvent*)
{

}
void MainWindow::slotAuthor()
{
    QString text = "<h3>WTF_Viewer " + VERSION + "</h3> <br>"
                   "WTF(What flies?)<br>"
                   "Author: Verbkin Mikhail <br>"
                   "Email: <a href=\"mailto:verbkinm@yandex.ru\" >verbkinm@yandex.ru</a> <br>"
                   "Source code: <a href='https://github.com/verbkinm/wtf_viewer'>github.com</a> <br><br>"
                   "The program for my dear friend! =))";

    QMessageBox::about(this, "Author", text);
}

void MainWindow::slotPlotGraph()
{
    Frames* frames = pViewerWidget->getFrames();
    GraphDialog* gd = new GraphDialog(frames, this);
    QString legendText;
    QString chartTitle = "Graph ";

    connect(gd,          SIGNAL(signalDataXChanged(QString)), this,
                         SLOT(slotGrapgWindowCheck(QString)));

    //наполняем список GraphDialog существующими графиками
    foreach (CentralWidget* cw, graphWindowList)
        gd->appendWindow(cw->getTitle());

    emit gd->signalDataXChanged(gd->getCurrentX());

    if(gd->exec() == QDialog::Accepted)
    {
//        QApplication::setOverrideCursor(Qt::WaitCursor);

        QVector<QPointF> vector;
        if(gd->getCurrentX() == "Tots")
        {

            vector = frames->getClusterVectorTot(gd->getCurrentClusterLenght());

            legendText = gd->getCurrentY() + "px";

//            for (auto &item : vector) {
//                qDebug() << item.x() << item.y();
//            }

        }
        if(gd->getCurrentX() == "Clusters")
        {
            vector = frames->getClusterVector();
            legendText = currentActiveFile;
        }
        if(gd->getCurrentX() == "Energy")
        {
            QMessageBox::information(this, "oooooops", "Kiss my ass, my little unicorn! =))");
            return;
        }

        if(graphWindowList.length() == 0 || gd->getCurrentWindowGraph() == gd->NEW_WINDOW)
        {
            CentralWidget* graphWindow = new CentralWidget(this);
            graphWindow->addSeries(vector, legendText, gd->getCurrentX(), "Count");
            graphWindowList.append(graphWindow);
            graphWindow->setTitle(chartTitle + QString::number(graphWindowList.count()));

            connect(graphWindow, SIGNAL(signalCloseWindow(QObject*)), this,
                                 SLOT(slotCloseGraphWindow(QObject*)));

            graphWindow->showMaximized();
        }
        else
        {
            CentralWidget* graphWindow = nullptr;

            foreach (CentralWidget* cw, graphWindowList)
                if(cw->getTitle() == gd->getCurrentWindowGraph())
                    graphWindow = cw;

            graphWindow->addSeries(vector, legendText, gd->getCurrentX(), "Count");
            graphWindow->show();
        }

//        QApplication::restoreOverrideCursor();
    }

    delete gd;
}

void MainWindow::slotGeneralCalibration()
{
    GeneralCalibration gc(settings, this);
    if(gc.exec() == QDialog::Accepted)
    {
        gc.writeSettings();
    }
}

void MainWindow::slotPixelCalibration()
{

}

void MainWindow::slotSettingsImage()
{
    pSettingsImage = new SettingsImage(settings, this);
    if(pSettingsImage->exec() == QDialog::Accepted)
        pSettingsImage->writeSettings();

    delete pSettingsImage;
    pSettingsImage = nullptr;
}
void MainWindow::slotSelectFile(const QModelIndex& index)
{
    QFileInfo file(_fs_model.filePath(index));
    currentActiveFile = file.fileName();

    this->statusBar()->showMessage(_fs_model.filePath(index));
    pViewerWidget->setImageFile(_fs_model.filePath(index));
    if(_treeView.isExpanded(index))
        _treeView.collapse(index);
    else
        _treeView.expand(index);

    if(file.suffix() == "clog")
        _menuGraph.setEnabled(true);
    else
        _menuGraph.setDisabled(true);

//Начало тормозить  дерево - использовать QAbstractProxyModel
    if(_fs_model.isDir(index))
        settings.setValue("Path/lastDir", _fs_model.filePath(index) );
    else
        settings.setValue("Path/lastDir", _fs_model.filePath(index.parent()) );
}

bool MainWindow::event(QEvent *event)
{
//    qDebug() << event->type();
    return QWidget::event(event);
}
MainWindow::~MainWindow()
{
    delete pViewerWidget;

    for (auto *cw : graphWindowList)
        delete cw;
}
