#include <QPushButton>
#include <QTransform>
#include <QFileDialog>
#include <QTabBar>
#include <QPixmap>
#include <QMessageBox>
#include <QFile>
#include <QFileInfo>

#include <QDebug>
#include <QTime>

#include "viewer_widget.h"
#include "ui_viewer_widget.h"
#include "checkFile/checkfile.h"

Viewer_widget::Viewer_widget(std::shared_ptr<const QSettings> spSetting, QWidget *parent) :
    QWidget(parent), /*_settings(&setting)*/
    ui(new Ui::Viewer_widget)
{
    _spSettings = spSetting;
    ui->setupUi(this);

    makeMaskTab();

    ui->graphicsView->setSettings(spSetting);
    graphicsView_origin.setScene(ui->graphicsView->getScenePtr());

    graphicsView_origin.hideAllPanel();
    graphicsView_origin.setReadOnly(true);
    graphicsView_origin.hideSettingsButton(true);
    mask_viewer.hideAllPanel();

    connect(&mask_settings, SIGNAL(signalOpenTXT(QString)), &mask_viewer, SLOT(slotSetImageFile(QString)));
    connect(&mask_settings, SIGNAL(signalSaveTXT()), &mask_viewer, SLOT(slotSaveTXT()));
    connect(&mask_settings, SIGNAL(signalGenerated(QString)), &mask_viewer, SLOT(slotSetImageFile(QString)));

    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(slotTabChanged(int)));

    //следующие две строчки - чтобы сразу отлавливались сигналы при перемещении курсора над View
    ui->tabWidget->setCurrentIndex(1);
    ui->tabWidget->setCurrentIndex(0);
}

Viewer_widget::~Viewer_widget()
{
    delete ui;
}

void Viewer_widget::setImageFile(QString &path)
{
    ui->graphicsView->setImageFile(path);
}

std::pair<const Frames &, bool> Viewer_widget::getFrames()
{
    return ui->graphicsView->getFrames();
}

QImage Viewer_widget::getImageFromTxtFile(const QString &file)
{
//    return ui->graphicsView->getImageFromTxtFile(file);
}

void Viewer_widget::makeMaskTab()
{
    main_splitter.setOrientation(Qt::Horizontal);
    left_splitter.setOrientation(Qt::Vertical);
    right_splitter.setOrientation(Qt::Vertical);

    main_splitter.addWidget(&left_splitter);
    main_splitter.addWidget(&right_splitter);

    left_splitter.addWidget(&graphicsView_origin);
    left_splitter.addWidget(&mask_viewer);
    right_splitter.addWidget(&graphicsView_Result);
    right_splitter.addWidget(&mask_settings);

    ui->tab_2->layout()->addWidget(&main_splitter);
    graphicsView_Result.hideAllPanel();

    main_splitter.setStretchFactor(0,1);
}

void Viewer_widget::slotTabChanged(int value)
{
    /* при смене вкладки на Mask делаем так, чтобы на этой вкладке на виджете graphicsView
     * нельзя было рисовать, создавать и перемещать рамку
     */
    if(value == 1)
        ui->graphicsView->setReadOnly(true);
    else if(value == 0)
        ui->graphicsView->setReadOnly(false);
}
