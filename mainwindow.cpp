#include <QApplication>
#include <QHeaderView>
#include <QMessageBox>
#include <QStatusBar>
#include <QDebug>
#include <QtMath>

#include "mainwindow.h"
#include "export\export.h"
#include "calibration/generalcalibration.h"
#include "viewer_widget/viewer/viewer_processor/viewer_txt_processor.h"
#include "settings/settingsimage.h"
#include "settings/settingsclogfile.h"
#include "settings/batch_processing.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent),
    settings(std::make_shared<QSettings>(QSettings::IniFormat, QSettings::UserScope, "WTF.org", "WTF")),
    _viewerWidget(settings, this),
    _programVersion("0.9.10.4")
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
    _menuFile.addAction(QIcon(":/batch"), "Batch processing", this, SLOT(slotBatchProcessing()));
    _menuFile.addSeparator();
    _menuFile.addAction(QIcon(":/exit"), "Exit", QApplication::instance(), SLOT(quit()));
    _menuSettings.setTitle("Settings");
    _menuSettings.addAction(QIcon(":/image"), "Image", this, SLOT(slotSettingsImage()));
    _menuSettings.addAction(QIcon(":/image"), "Open clog file settings", this, SLOT(slotSettingsOpenClogFile()));
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
    if(graphDialog.exec() == QDialog::Accepted)
    {
        QString legendText;
        QString chartTitle = "Graph ";

        std::map<float, float> map = createVectorAccordingGraphType(graphDialog, legendText, frames);
        if(_graphWindowMap.size() == 0 || graphDialog.getCurrentWindowGraph() == graphDialog._NEW_WINDOW)
        {
            CentralWidget* graphWindow = new CentralWidget(this);
            graphWindow->addSeries(map, legendText, graphDialog.getType(), "Count");
            graphWindow->setTitle(chartTitle + QString::number(_graphWindowMap.size()));
            graphWindow->showMaximized();
            _graphWindowMap.push_back(graphWindow);
            connect(graphWindow, &CentralWidget::signalCloseWindow, this, &MainWindow::slotCloseGraphWindow);
        }
        else
        {
            CentralWidget* graphWindow = nullptr;
            for (auto &item : _graphWindowMap)
                if(item->getTitle() == graphDialog.getCurrentWindowGraph())
                    graphWindow = item;
            graphWindow->addSeries(map, legendText, graphDialog.getType(), "Count");
            graphWindow->showMaximized();
        }
    }
}

void MainWindow::slotExportFiles()
{
    QFileInfo file(_fs_model.filePath(_treeView.currentIndex()));
    if(file.isDir())
        exportingFiles(file.absoluteFilePath());
    else if(file.isFile())
        exportingFiles(file.absolutePath());
}

void MainWindow::slotBatchProcessing()
{
    if(_currentFile.suffix() != "clog")
    {
        QMessageBox::warning(this, "Warning", "Not selected CLOG file");
        return;
    }

    std::pair<const Frames &, bool> pair = _viewerWidget.getFrames();
    if(!pair.second)
    {
        qDebug() << __FUNCTION__ << "frames size < 0";
        return;
    }

    Batch_Processing bp(_currentFile, pair.first, this);
    if(bp.exec() == QDialog::Accepted)
    {
        bp.slice();
    }
}

void MainWindow::slotGrapgWindowCheck(const QString &data)
{
    GraphDialog* gd = static_cast<GraphDialog*>(sender());
    gd->clearWindow();
    for (auto &item : _graphWindowMap)
    {
        if(item->getDataXType() == data)
            gd->appendWindow(item->getTitle());
    }
    gd->selectLastWindow();
}

std::map<float, float> MainWindow::createVectorAccordingGraphType(GraphDialog &graphDialog, QString &legendText, const Frames &frames)
{
    std::map<float, float> map;
    if(graphDialog.getType() == "Tots")
    {
        legendText = graphDialog.getClusterSize() + "px";
        if(graphDialog.getClusterSize() == "All")
            return frames.getMapOfTotPointsSummarize(Frames::ALL_CLUSTER);
        else
            return frames.getMapOfTotPointsSummarize(graphDialog.getClusterSize().toULongLong());
    }
    else if(graphDialog.getType() == "Clusters")
    {
        return frames.getMapOfClusterSize();
    }


    //    else if(graphDialog.getType() == "Energy")
    //    {
    //        legendText = graphDialog.getClusterSize() + "px";

    //        double A = (settings->value("GeneralCalibration/A").toDouble());
    //        double B = (settings->value("GeneralCalibration/B").toDouble());
    //        double C = (settings->value("GeneralCalibration/C").toDouble());
    //        double T = (settings->value("GeneralCalibration/T").toDouble());

    //        if(graphDialog.getClusterSize() == "All")
    //        {
    //           return frames.getMapOfEnergySum(Frames::ALL_CLUSTER, A, B, C, T);

    ////            std::map<double, double> energy;

    ////            for(auto [key, value] : tots)
    ////            {
    ////                double parA = - A;
    ////                double parB = key + A * T - B;
    ////                double parC = -(key * T)  + B * T + C;

    ////                double newValue = ( -parB - ( qSqrt((parB * parB) - (4 * parA * parC))) ) / (2 * parA);

    ////                energy[value] = newValue;
    ////            }
    ////            return energy;
    //        }
    //        else
    //        {
    //            std::map<double, double> tots = frames.getMapOfTotPoints(graphDialog.getClusterSize().toULongLong());
    //            std::map<double, double> energy;

    //            for(auto [key, value] : tots)
    //            {
    //                double parA = - A;
    //                double parB = key + A * T - B;
    //                double parC = -(key * T)  + B * T + C;

    //                double newValue = ( -parB - ( qSqrt((parB * parB) - (4 * parA * parC))) ) / (2 * parA);

    //                energy[value] = newValue;
    //            }
    //            return energy;
    //        }
    //    }
    return map;
}

void MainWindow::slotCloseGraphWindow(QObject *obj)
{
    _graphWindowMap.remove(static_cast<CentralWidget*>(obj));
    delete obj;
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
    GraphDialog graphDialog(settings, frames, this);
    connect(&graphDialog, &GraphDialog::signalDataXChanged, this, &MainWindow::slotGrapgWindowCheck);

    //наполняем список GraphDialog существующими графиками
    for(auto &item : _graphWindowMap)
        graphDialog.appendWindow(item->getTitle());

    emit graphDialog.signalDataXChanged(graphDialog.getType());
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

void MainWindow::slotSettingsOpenClogFile()
{
    SettingsClogFile scf(settings, this);
    if(scf.exec() == QDialog::Accepted)
        scf.writeSettings();
}
void MainWindow::slotSelectFile(const QModelIndex& index)
{
    _currentFile.setFile(_fs_model.filePath(index));

    this->statusBar()->showMessage(_fs_model.filePath(index));
    QString fileName = _fs_model.filePath(index);
    _viewerWidget.setImageFile(fileName);
    if(_treeView.isExpanded(index))
        _treeView.collapse(index);
    else
        _treeView.expand(index);

    if(_currentFile.suffix() == "clog")
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
    _graphWindowMap.erase(_graphWindowMap.begin(), _graphWindowMap.end());
}
