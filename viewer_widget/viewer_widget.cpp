#include <QDebug>

#include "viewer_widget.h"
#include "ui_viewer_widget.h"
#include "checkFile/checkfile.h"

Viewer_widget::Viewer_widget(std::shared_ptr<const QSettings> spSetting, QWidget *parent) :
    QWidget(parent), _spSettings(spSetting),
    ui(new Ui::Viewer_widget)
{
    ui->setupUi(this);

    makeMaskTab();

    ui->graphicsView->setSettings(spSetting);
    _graphicsView_origin.setScene(ui->graphicsView->getScenePtr());

    _graphicsView_origin.hideAllPanel();
    _graphicsView_origin.hideSettingsButton(true);
    _mask_viewer.hideAllPanel();

    connect(&_mask_settings, SIGNAL(signalOpenTXT(QString)), &_mask_viewer, SLOT(slotSetImageFile(QString)));
    connect(&_mask_settings, SIGNAL(signalSaveTXT()), &_mask_viewer, SLOT(slotSaveTXT()));
    connect(&_mask_settings, SIGNAL(signalGenerated(QString)), &_mask_viewer, SLOT(slotSetImageFile(QString)));
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), SLOT(slotTabChanged(int)));
}

Viewer_widget::~Viewer_widget()
{
    delete ui;
}

void Viewer_widget::setImageFile(const QString &path)
{
    ui->graphicsView->setImageFileName(path);
}

std::pair<const Frames &, bool> Viewer_widget::getFrames() const
{
    return ui->graphicsView->getFrames();
}

void Viewer_widget::makeMaskTab()
{
    _main_splitter.setOrientation(Qt::Horizontal);
    _left_splitter.setOrientation(Qt::Vertical);
    _right_splitter.setOrientation(Qt::Vertical);

    _main_splitter.addWidget(&_left_splitter);
    _main_splitter.addWidget(&_right_splitter);

    _left_splitter.addWidget(&_graphicsView_origin);
    _left_splitter.addWidget(&_mask_viewer);
    _right_splitter.addWidget(&_graphicsView_Result);
    _right_splitter.addWidget(&_mask_settings);

    ui->tab_2->layout()->addWidget(&_main_splitter);
    _graphicsView_Result.hideAllPanel();

    _main_splitter.setStretchFactor(0,1);
}

void Viewer_widget::slotTabChanged(int value)
{
    /* при смене вкладки на Mask делаем так, чтобы на этой вкладке на виджете graphicsView
     * нельзя было рисовать, создавать и перемещать рамку
     */
    if(value == MASK)
        ui->graphicsView->setSceneReadOnly(true);
    else if(value == VIEW_AND_EDIT)
        ui->graphicsView->setSceneReadOnly(false);
}
