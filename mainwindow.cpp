#include <QApplication>
#include <QHeaderView>
#include <QMessageBox>
#include <QStatusBar>
#include <QDebug>

#include "mainwindow.h"
#include "export\export.h"
#include "calibration/generalcalibration.h"
#include "viewer_widget/viewer/viewer_processor/viewer_txt_processor.h"
#include "settings/settingsimage.h"


MainWindow::MainWindow(QWidget *parent): QMainWindow(parent),
    settings(std::make_shared<QSettings>(QSettings::IniFormat, QSettings::UserScope, "WTF.org", "WTF")),
    _viewerWidget(settings, this),
    _programVersion("0.9.8.13")
{
    settings.get()->setIniCodec("UTF-8");
    _splitter.setOrientation(Qt::Horizontal);
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
    _splitter.addWidget(&_viewerWidget);
    this->setCentralWidget(&_splitter);
    this->setWindowIcon(QIcon(":/atom"));
    this->setWindowTitle("WTF_Viewer " + _programVersion);
    createMenu();
    openLastDir();
    connect(&_treeView, &QAbstractItemView::clicked, this, &MainWindow::slotSelectFile);
    connect(&_treeView, &QAbstractItemView::activated, this, &MainWindow::slotSelectFile);
}
void MainWindow::createMenu()
{
    _menuFile.setTitle("File");
    _menuFile.addAction(QIcon(":/save_as"), "Export files", this, SLOT(slotExportFiles()));
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
    QModelIndex lastDirIndex = _fs_model.index(settings->value("Path/lastDir").toString());
    _treeView.expand(lastDirIndex);
    QModelIndex index = lastDirIndex;
    while (index.parent().isValid())
    {
        QModelIndex parent = index.parent();
        _treeView.expand(parent);
        index = parent;
    }
}

void MainWindow::saveAccordingOptions(int options, int &error, int &correct, QImage &image, const QString &fullName)
{
    switch (options)
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

void MainWindow::exportingFiles(const QString &path)
{
    Export exportWindow(path, this);
    if(exportWindow.exec() == QDialog::Accepted)
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        int correct = 0;
        int error = 0;
        QStringList listFiles = exportWindow.getFileNames();
        for (auto &fileName : listFiles)
        {
            Viewer_Txt_Processor viewerTxtProc;
            viewerTxtProc.setFileName(fileName);
            QImage image(viewerTxtProc.getImage());
            if(image.format() != QImage::Format_Invalid)
            {
                QFileInfo fileInfo(fileName);
                QFileInfo resultFile(QDir(exportWindow.getPath()), fileInfo.baseName());
                QString fullName = resultFile.absoluteFilePath();
                saveAccordingOptions(exportWindow.getOption(), error, correct, image, fullName);
            }
            else
                error++;
        }
        QApplication::restoreOverrideCursor();
        QMessageBox::information(this, "Export", "Export " + QString::number(correct) +
                                 " file(s)<br>Error export " + QString::number(error) + " file(s)");
    }
}

void MainWindow::graphDialogExec(GraphDialog &graphDialog, const Frames &frames)
{
    QString legendText;
    QString chartTitle = "Graph ";
    if(graphDialog.exec() == QDialog::Accepted)
    {
        std::vector<QPointF> pointVector = createVectorAccordingGraphType(graphDialog, legendText, frames);
        if(_graphWindowMap.size() == 0 || graphDialog.getCurrentWindowGraph() == graphDialog._NEW_WINDOW)
        {
            CentralWidget* graphWindow = new CentralWidget(this);
            graphWindow->addSeries(pointVector, legendText, graphDialog.getCurrentX(), "Count");
            graphWindow->setTitle(chartTitle + QString::number(_graphWindowMap.size()));
            graphWindow->showMaximized();
            _graphWindowMap[graphWindow->getTitle()] = graphWindow;
            connect(graphWindow, &CentralWidget::signalCloseWindow, this, &MainWindow::slotCloseGraphWindow);
        }
        else
        {
            CentralWidget* graphWindow = nullptr;
            for (auto [key, value] : _graphWindowMap)
                if(key == graphDialog.getCurrentWindowGraph())
                    graphWindow = value;
            graphWindow->addSeries(pointVector, legendText, graphDialog.getCurrentX(), "Count");
            graphWindow->showMaximized();
        }
    }
}

std::vector<QPointF> MainWindow::createVectorAccordingGraphType(GraphDialog &graphDialog, QString &legendText, const Frames &frames)
{
    std::vector<QPointF> pointVector;
    if(graphDialog.getCurrentX() == "Tots")
    {
        legendText = graphDialog.getCurrentY() + "px";
//            vector = frames.get
    }
    if(graphDialog.getCurrentX() == "Clusters")
    {
        legendText = _currentActiveFile;
//            vector = frames.get
    }
    if(graphDialog.getCurrentX() == "Energy")
    {
//        QMessageBox::information(this, "oooooops", "Kiss my ass, my little unicorn! =))");
//        return;
    }
    return pointVector;
}
void MainWindow::slotExportFiles()
{
    QFileInfo file(_fs_model.filePath(_treeView.currentIndex()));
    if(file.isDir())
        exportingFiles(file.absoluteFilePath());
    else if(file.isFile())
        exportingFiles(file.absolutePath());
}

void MainWindow::slotCloseGraphWindow(QObject *obj)
{
    _graphWindowMap.erase(static_cast<CentralWidget*>(obj)->getTitle());
    delete obj;
}

void MainWindow::slotGrapgWindowCheck(const QString &data)
{
    GraphDialog* gd = static_cast<GraphDialog*>(sender());
    gd->clearWindow();
    for (auto [key, value] : _graphWindowMap)
    {
        if(value->getDataXType() != data)
            continue;
        gd->appendWindow(key);
    }
    gd->selectLastWindow();
}

void MainWindow::slotAuthor()
{
    QString text = "<h3>WTF_Viewer " + _programVersion + "</h3> <br>"
                   "WTF(What flies?)<br>"
                   "Author: Verbkin Mikhail <br>"
                   "Email: <a href=\"mailto:verbkinm@yandex.ru\" >verbkinm@yandex.ru</a> <br>"
                   "Source code: <a href='https://github.com/verbkinm/wtf_viewer'>github.com</a> <br><br>"
                   "The program for my dear friend! =))";
    QMessageBox::about(this, "Author", text);
}

void MainWindow::slotPlotGraph()
{
    if(!_viewerWidget.getFrames().second)
        return;

    const Frames &frames = _viewerWidget.getFrames().first;
    GraphDialog graphDialog(frames, this);
    connect(&graphDialog, &GraphDialog::signalDataXChanged, this, &MainWindow::slotGrapgWindowCheck);

    //наполняем список GraphDialog существующими графиками
    for(auto item : _graphWindowMap)
        graphDialog.appendWindow(item.first);

    emit graphDialog.signalDataXChanged(graphDialog.getCurrentX());
    graphDialogExec(graphDialog, frames);
}

void MainWindow::slotGeneralCalibration()
{
    GeneralCalibration gc(settings, this);
    if(gc.exec() == QDialog::Accepted)
        gc.writeSettings();
}

void MainWindow::slotPixelCalibration()
{

}

void MainWindow::slotSettingsImage()
{
    SettingsImage settingsImage(settings, this);
    if(settingsImage.exec() == QDialog::Accepted)
        settingsImage.writeSettings();
}
void MainWindow::slotSelectFile(const QModelIndex& index)
{
    QFileInfo file(_fs_model.filePath(index));
    _currentActiveFile = file.fileName();

    this->statusBar()->showMessage(_fs_model.filePath(index));
    QString fileName = _fs_model.filePath(index);
    _viewerWidget.setImageFile(fileName);
    if(_treeView.isExpanded(index))
        _treeView.collapse(index);
    else
        _treeView.expand(index);

    if(file.suffix() == "clog")
        _menuGraph.setEnabled(true);
    else
        _menuGraph.setDisabled(true);

    if(_fs_model.isDir(index))
        settings.get()->setValue("Path/lastDir", _fs_model.filePath(index) );
    else
        settings.get()->setValue("Path/lastDir", _fs_model.filePath(index.parent()) );
}

MainWindow::~MainWindow()
{
    for (auto item : _graphWindowMap)
        delete item.second;
    _graphWindowMap.clear();
}
