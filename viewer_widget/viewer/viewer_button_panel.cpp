#include "viewer_button_panel.h"
#include "ui_viewer_button_panel.h"

Viewer_Button_Panel::Viewer_Button_Panel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Viewer_Button_Panel)
{
    ui->setupUi(this);

    connect(ui->save_bmp, SIGNAL(clicked()), SIGNAL(signalSaveToBmp()));
    connect(ui->save_txt, SIGNAL(clicked()), SIGNAL(signalSaveToTxt()));

    connect(ui->horizontal_mirror, SIGNAL(clicked()), SIGNAL(signalMirrorHorizontal()));
    connect(ui->vertical_mirror, SIGNAL(clicked()), SIGNAL(signalMirrorVertical()));

    connect(ui->rotate_plus, SIGNAL(clicked()), SIGNAL(signalRotatePlus()));
    connect(ui->rotate_minus, SIGNAL(clicked()), SIGNAL(signalRotateMinus()));

    connect(ui->scaled_plus, SIGNAL(clicked()), SIGNAL(signalScaledPlus()));
    connect(ui->scaled_minus, SIGNAL(clicked()), SIGNAL(signalScaledMinus()));

    connect(ui->angle, SIGNAL(valueChanged(double)), SIGNAL(signalAngle(double)));
    connect(ui->reset_transform, SIGNAL(clicked()), SIGNAL(signalResetTransform()));
}

Viewer_Button_Panel::~Viewer_Button_Panel()
{
    delete ui;
}
