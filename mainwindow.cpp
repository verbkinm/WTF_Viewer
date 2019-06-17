#include "mainwindow.h"
#include "export\export.h"
#include "graph/graphdialog.h"

#include <QApplication>
#include <QHeaderView>
#include <QDebug>

const QString VERSION =  "0.9.8";

#ifdef Q_OS_Linux
    #define SPLITTER_PATH "/"
#endif
#ifdef Q_OS_WIN
    #define SPLITTER_PATH "\\"
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), settings(QSettings::IniFormat, QSettings::UserScope, "WTF.org", "WTF")
{
    settings.setIniCodec("UTF-8");

    createMenu();

    pSplitter       = new QSplitter(Qt::Horizontal);
    pFSModel        = new QFileSystemModel;
    pTreeView       = new QTreeView;
    pViewerWidget   = new Viewer_widget(settings, this);
    pEventFilter    = new EventFilter(pTreeView);
    pTreeView->installEventFilter(pEventFilter);


    pFSModel->setRootPath(QDir::rootPath());
    QStringList filter;
    filter << "*.txt" << "*.clog";
    pFSModel->setNameFilters(filter);
    pFSModel->setNameFilterDisables(false);

    pTreeView->setModel(pFSModel);
    pTreeView->header()->hideSection(1);
    pTreeView->header()->hideSection(2);
    pTreeView->header()->hideSection(3);
    pTreeView->setAnimated(true);

    pSplitter->addWidget(pTreeView);
    pSplitter->addWidget(pViewerWidget);

    this->setCentralWidget(pSplitter);

    connect(pTreeView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(slotSelectFile(const QModelIndex&)));
    connect(pTreeView, SIGNAL(activated(const QModelIndex&)), this, SLOT(slotSelectFile(const QModelIndex&)));

    this->setWindowIcon(QIcon(":/atom"));

    this->setWindowTitle("WTF_Viewer " + VERSION);

    QString lastPath = settings.value("Path/lastDir", 0).toString();
    pTreeView->setCurrentIndex(pFSModel->index(lastPath));
    pTreeView->clicked(pTreeView->currentIndex());

}
void MainWindow::createMenu()
{
    pMenuFile = new QMenu("File");
    pMenuFile->addAction(QIcon(":/save_as"), "Export files", this, SLOT(slotExportFile()));

    pMenuFile->addSeparator();

    pMenuFile->addAction(QIcon(":/exit"), "Exit", QApplication::instance(), SLOT(quit()));

    pMenuSettings = new QMenu("Settings");
    pMenuSettings->addAction(QIcon(":/image"), "Image", this, SLOT(slotSettingsImage()));

    pMenuGraph = new QMenu("Graph");
    pMenuGraph->addAction(QIcon(":/graph"), "Plot the graph", this, SLOT(slotPlotGraph()));
    pMenuGraph->setDisabled(true);

    pMenuAbout= new QMenu("About");
    pMenuAbout->addAction(QIcon(":/author"),"Author", this, SLOT(slotAuthor()));
    pMenuAbout->addAction(QIcon(":/qt_logo"), "About Qt", QApplication::instance(), SLOT(aboutQt()));

    this->menuBar()->addMenu(pMenuFile);
    this->menuBar()->addMenu(pMenuSettings);
    this->menuBar()->addMenu(pMenuGraph);
    this->menuBar()->addMenu(pMenuAbout);
}
void MainWindow::slotExportFile()
{
    QFileInfo file(pFSModel->filePath(pTreeView->currentIndex()));
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

        foreach (QString fileName, listFiles) {
            QImage image(pViewerWidget->getImageFromTxtFile(fileName));
            if(image.format() != QImage::Format_Invalid)
            {
                QFileInfo fileInfo(fileName);
                QString fullName = exportWindow.getPath() + SPLITTER_PATH + fileInfo.baseName();
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

        QMessageBox::information(this, "Export", "Export " + QString::number(correct) + " file(s)<br>"
                                                                                        "Error export " + QString::number(error) + " file(s)");
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

    foreach (CentralWidget* cw, graphWindowList)
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
    QFileInfo file(pFSModel->filePath(index));
    currentActiveFile = file.fileName();

    this->statusBar()->showMessage(pFSModel->filePath(index));
    pViewerWidget->setImageFile(pFSModel->filePath(index));
    if(pTreeView->isExpanded(index))
        pTreeView->collapse(index);
    else
        pTreeView->expand(index);

    if(file.suffix() == "clog")
        pMenuGraph->setEnabled(true);
    else
        pMenuGraph->setDisabled(true);

//Начало тормозить  дерево - использовать QAbstractProxyModel
    if(pFSModel->isDir(index))
        settings.setValue("Path/lastDir", pFSModel->filePath(index) );
}

bool MainWindow::event(QEvent *event)
{
//    qDebug() << event->type();
    return QWidget::event(event);
}
MainWindow::~MainWindow()
{
    delete pViewerWidget;
    delete pFSModel;
    delete pEventFilter;
    delete pTreeView;
    delete pSplitter;

    delete pMenuFile;
    delete pMenuAbout;

    foreach (CentralWidget* cw, graphWindowList)
        delete cw;
}
