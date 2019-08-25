#include "panelwidget.h"
#include "ui_panelwidget.h"

#include "centralwidget.h"

#include <QChart>
#include <QXYSeries>
#include <QColorDialog>
#include <QValueAxis>
#include <QMessageBox>
#include <QFileDialog>

#include <QDebug>


PanelWidget::PanelWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PanelWidget)
{
    ui->setupUi(this);

    ui->themeComboBox->addItem("Light", QChart::ChartThemeLight);
    ui->themeComboBox->addItem("Blue Cerulean", QChart::ChartThemeBlueCerulean);
    ui->themeComboBox->addItem("Dark", QChart::ChartThemeDark);
    ui->themeComboBox->addItem("Brown Sand", QChart::ChartThemeBrownSand);
    ui->themeComboBox->addItem("Blue NCS", QChart::ChartThemeBlueNcs);
    ui->themeComboBox->addItem("High Contrast", QChart::ChartThemeHighContrast);
    ui->themeComboBox->addItem("Blue Icy", QChart::ChartThemeBlueIcy);
    ui->themeComboBox->addItem("Qt", QChart::ChartThemeQt);

    ui->tickCountX->setValue(5);
    ui->tickCountY->setValue(5);

    connect(ui->themeComboBox, SIGNAL(currentIndexChanged(int)), SIGNAL(signalChangeTheme(int)));
    connect(ui->pointVisible, SIGNAL(toggled(bool)), SLOT(slotSetPointVisible(bool)));
    connect(ui->legendPosition, SIGNAL(currentIndexChanged(int)), SIGNAL(signalSetLegendPosition(int)));
    connect(ui->title, SIGNAL(textChanged(QString)), SIGNAL(signalSetTitile(QString)));
    connect(ui->animation, SIGNAL(toggled(bool)), SIGNAL(signalAnimation(bool)));
    connect(ui->antialiasing, SIGNAL(toggled(bool)), SIGNAL(signalAntialiasing(bool)));
    connect(ui->seriesList, SIGNAL(currentIndexChanged(int)), SLOT(slotSetSeriesPropery(int)));
    connect(ui->colorSelect, SIGNAL(clicked()), SLOT(slotSetSeriesColor()));
    connect(ui->seriesType, SIGNAL(currentIndexChanged(int)), SLOT(slotSetSeriesType(int)));
    connect(ui->hide, SIGNAL(toggled(bool)), SLOT(slotHideSeries(bool)));
    connect(ui->tickCountX, SIGNAL(valueChanged(int)), SIGNAL(signalTickCountChangeX(int)));
    connect(ui->tickCountY, SIGNAL(valueChanged(int)), SIGNAL(signalTickCountChangeY(int)));
    connect(ui->penWidth, SIGNAL(valueChanged(int)), SLOT(slotSetSeriesPenWidth(int)));
    connect(ui->deleteSeries, SIGNAL(clicked()), SLOT(slotDeleteSeries()));
    connect(ui->saveToCSV, SIGNAL(clicked()), SLOT(slotSaveToCSV()));
}

PanelWidget::~PanelWidget()
{
    delete ui;
    for (auto series :seriesList)
        delete series;
    for (auto model : modelList)
        delete model;
}

void PanelWidget::addSeriesList(QXYSeries *series)
{
    seriesList.append(series);
    CustomTableModel* model = new CustomTableModel();
    model->setVector(series->pointsVector());
    modelList.append(model);
    ui->seriesList->addItem(series->name());
}

QList<QXYSeries *>* PanelWidget::getSeriesList()
{
    return &seriesList;
}

void PanelWidget::createTableData()
{
    CustomTableModel* model = modelList.at(ui->seriesList->currentIndex());
    ui->tableView->setModel(model);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    mapper.setXColumn(0);
    mapper.setYColumn(1);
    mapper.setSeries(currentSeries);
    mapper.setModel(model);

    QString seriesColorHex = "#000000";

    seriesColorHex = "#" + QString::number(currentSeries->pen().color().rgb(), 16).right(6).toUpper();
    model->addMapping(seriesColorHex, QRect(0, 0, 2, model->rowCount()));
}

void PanelWidget::setDisableUnits(bool value)
{
    ui->tab_2->setDisabled(value);
    ui->seriesType->setDisabled(value);
}

int PanelWidget::getSeriesType()
{
    int type = ui->seriesType->currentIndex();
    switch (type) {
    case 0:
        type = QXYSeries::SeriesTypeLine;
        break;
    case 1:
        type = QXYSeries::SeriesTypeSpline;
        break;
    case 2:
        type = QXYSeries::SeriesTypeScatter;
        break;
    default:
        type = QXYSeries::SeriesTypeLine;
        break;
    }

    return type;
}

void PanelWidget::setTitle(QString title)
{
    ui->title->setText(title);
}

void PanelWidget::slotSetSeriesPropery(int value)
{
    if(value < 0)
    {
        currentSeries = nullptr;
        setDisableUnits(true);

        return;
    }
    else
        setDisableUnits(false);

    QXYSeries* series = seriesList.at(value);
    QPixmap pix(16, 16);
    pix.fill(series->color());
    ui->colorSelect->setIcon(QIcon(pix));

    currentSeries = series;

    ui->pointVisible->setChecked(currentSeries->pointsVisible());
    ui->hide->setChecked(!currentSeries->isVisible());

    if(currentSeries->type() == QAbstractSeries::SeriesTypeScatter)
    {
        QScatterSeries* scatSer = static_cast<QScatterSeries*>(currentSeries);
        ui->penWidth->setValue(int(scatSer->markerSize()));
    }
    else
        ui->penWidth->setValue(currentSeries->pen().width());

    createTableData();
}

void PanelWidget::slotSetPointVisible(bool value)
{
    currentSeries->setPointsVisible(value);
}

void PanelWidget::slotSetSeriesColor()
{
    QColor color = QColorDialog::getColor(currentSeries->pen().color(), this);
    if(color.spec())
    {
        currentSeries->setColor(color);
        if(currentSeries->type() == QAbstractSeries::SeriesTypeScatter)
        {
            QScatterSeries* scatSer = static_cast<QScatterSeries*>(currentSeries);
            scatSer->setBorderColor(color);
        }

        QPixmap pix(16, 16);
        pix.fill(color);
        ui->colorSelect->setIcon(QIcon(pix));

        //обновление цвета в TableView
        modelList.at(ui->seriesList->currentIndex())->updateColor(color);
    }
}

void PanelWidget::slotSetSeriesType(int value)
{
    QXYSeries* series = nullptr;

    QChart* chart = seriesList.at(0)->chart();

    QString titleX = chart->axes(Qt::Horizontal).back()->titleText();
    QString titleY = chart->axes(Qt::Vertical).back()->titleText();

    disconnect(ui->seriesList,  SIGNAL(currentIndexChanged(int)), this,
                                SLOT(slotSetSeriesPropery(int)));

    ui->seriesList->clear();

    connect(ui->seriesList,     SIGNAL(currentIndexChanged(int)), this,
                                SLOT(slotSetSeriesPropery(int)));

    for (int i = 0; i < seriesList.length(); ++i)
    {
        switch (value) {
        case 0:
            series = new QLineSeries;
            break;
        case 1:
            series = new QSplineSeries;
            break;
        case 2:
            series = new QScatterSeries;
            break;
        default:
            break;
        }

        QXYSeries* seriestmp = seriesList.at(i);
        seriesList.replace(i, series);

        *series << seriestmp->points();
        series->setColor(seriestmp->color());
        series->setName(seriestmp->name());
        if(series->type() == QAbstractSeries::SeriesTypeScatter)
        {
            QScatterSeries* scatSer = static_cast<QScatterSeries*>(series);
            scatSer->setMarkerSize(seriestmp->pen().width());
            scatSer->setBorderColor(seriestmp->pen().color());
            scatSer->setPen(seriestmp->pen());
        }
        else
            series->setPen(seriestmp->pen());

        series->setPointsVisible(seriestmp->pointsVisible());
        series->setVisible(seriestmp->isVisible());

        chart->removeSeries(seriestmp);
        delete seriestmp;
        chart->addSeries(series);

        ui->seriesList->addItem(series->name());
    }

    chart->createDefaultAxes();

    chart->axes(Qt::Horizontal).back()->setTitleText(titleX);
    chart->axes(Qt::Vertical).back()->setTitleText(titleY);

    (static_cast<QValueAxis*>(chart->axes(Qt::Horizontal).back()))->setTickCount(ui->tickCountX->value());
    (static_cast<QValueAxis*>(chart->axes(Qt::Vertical).back()))->setTickCount(ui->tickCountY->value());

    chart->axes(Qt::Horizontal).back()->setRange(ui->axisXRangeMin->value(), ui->axisXRangeMax->value());
    chart->axes(Qt::Vertical).back()->setRange(ui->axisYRangeMin->value(), ui->axisYRangeMax->value());

    emit signalSeriesTypeChange();
}

void PanelWidget::slotHideSeries(bool value)
{
    if(value)
        currentSeries->hide();
    else
        currentSeries->setVisible(true);
}

void PanelWidget::slotSetSeriesPenWidth(int value)
{
    if(currentSeries->type() == QAbstractSeries::SeriesTypeScatter)
    {
        QScatterSeries* scatSer = static_cast<QScatterSeries*>(currentSeries);
        scatSer->setMarkerSize(value);
        QPen pen = currentSeries->pen();
        pen.setWidth(value);
        scatSer->setPen(pen);
    }
    else
    {
        QPen pen = currentSeries->pen();
        pen.setWidth(value);
        currentSeries->setPen(pen);
    }
}

void PanelWidget::slotDeleteSeries()
{
    QMessageBox msgBox(this);
    msgBox.setText("Are you really delete this series?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setIcon(QMessageBox::Question);

    if(msgBox.exec() == QMessageBox::Ok)
    {
        QChart* chart = currentSeries->chart();
        chart->removeSeries(currentSeries);

        delete currentSeries;

        int index = ui->seriesList->currentIndex();

        delete modelList.at(index);


        seriesList.removeAt(index);
        modelList.removeAt(index);

        ui->seriesList->removeItem(index);
    }
    emit signalSeriesDeleted();
}

void PanelWidget::slotSaveToCSV()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    ("Save as CSV"),
                                                    QDir::rootPath() + currentSeries->name(),
                                                    "CSV (*.csv);;All files (*.*)");
    if(fileName != "")
    {
        QFile file(fileName);
        QTextStream writeStrime(&file);

        if(file.open(QFile::WriteOnly))
            foreach (QPointF point, currentSeries->points())
                writeStrime <<   point.x() << ";" << point.y() << "\n";

        file.close();
    }
}

void PanelWidget::on_actionSetRubberMode_triggered()
{
    if(sender()->objectName() == "rubberModeRectangle")
        emit signalRubberMode(QtCharts::QChartView::RectangleRubberBand);

    else if (sender()->objectName() == "rubberModeHorizontal")
        emit signalRubberMode(QtCharts::QChartView::HorizontalRubberBand);

    else if (sender()->objectName() == "rubberModeVertical")
        emit signalRubberMode(QtCharts::QChartView::VerticalRubberBand);
}

void PanelWidget::slotAxisXRangeChanged()
{
    emit signalAxisXRangeChanged(ui->axisXRangeMin->value(), ui->axisXRangeMax->value());
}

void PanelWidget::slotAxisYRangeChanged()
{
    emit signalAxisYRangeChanged(ui->axisYRangeMin->value(), ui->axisYRangeMax->value());
}

void PanelWidget::setRangeAxisX(qreal min, qreal max)
{
    ui->axisXRangeMin->setValue(min);
    ui->axisXRangeMax->setValue(max);
}

void PanelWidget::setRangeAxisY(qreal min, qreal max)
{
    ui->axisYRangeMin->setValue(min);
    ui->axisYRangeMax->setValue(max);
}
