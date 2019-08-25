#include "centralwidget.h"

#include <QPushButton>
#include <QFileDialog>
#include <QFileInfo>
#include <QDebug>
#include <QStatusBar>
#include <limits>

#include <QApplication>

CentralWidget::CentralWidget(QWidget *parent) : QMainWindow(parent),
    pAxisX(nullptr), pAxisY(nullptr),
    maxX(0), maxY(0), minX(std::numeric_limits<int>::max()), minY(std::numeric_limits<int>::max())
{
    panelWidget.setParent(this);
    createMenu();
    statusBar()->insertWidget(0, &statusBarWidget);
    setLinersXYDefault();

    QFont font = chart.titleFont();
    font.setPixelSize(18);
    chart.setTitleFont(QFont(font));

    createAxes();
    chart.axes(Qt::Horizontal).back()->setTitleText("X");
    chart.axes(Qt::Vertical).back()->setTitleText("Y");
    chart.setDropShadowEnabled(true);

    chartView.setChart(&chart);

    layout.addWidget(&chartView);
    layout.addWidget(&panelWidget);
    layout.setStretch(0, 1);

    centralWidget.setLayout(&layout);
    setCentralWidget(&centralWidget);
    chartView.setFocus();

    connectPanelWidgetSignals();
    connect(&chartView, &ChartView::signalMousePosition, this, &CentralWidget::slotViewXYCoordinate);
}

CentralWidget::~CentralWidget()
{
    delete pAxisX;
    delete pAxisY;
}

void CentralWidget::addSeries(std::vector<QPointF> &pointVector, QXYSeries::SeriesType type, QString legendTitle, QString axsisX_Title, QString axsisY_Title)
{
    QXYSeries* series = createSeriesAccordingType(type);
    series->setName(legendTitle);
    fillSeriesOfPoints(pointVector, series);
    setSeriesProperty(series);
    setChartViewXYRange();
    createAxes();
    setRangeAndTitleForAxes(axsisX_Title, axsisY_Title);

    chart.addSeries(series);
    panelWidget.addSeriesList(series);
}

void CentralWidget::addSeries(std::vector<QPointF> &pointVector, QString legendTitle, QString axsisX_Title, QString axsisY_Title)
{
    addSeries(pointVector, QXYSeries::SeriesType(panelWidget.getSeriesType()), legendTitle, axsisX_Title, axsisY_Title);
}

void CentralWidget::setTitle(QString title)
{
    chart.setTitle(title);
    panelWidget.setTitle(chart.title());
}

QString CentralWidget::getTitle() const
{
    return chart.title();
}

QString CentralWidget::getDataXType() const
{
    return chart.axes(Qt::Horizontal).back()->titleText();
}
void CentralWidget::setLinersXYDefault()
{
    lineSeriesX.setPen(QPen(QColor(Qt::black)));
    lineSeriesX << QPointF(0, 0) << QPointF(chart.maximumWidth(), 0);
    lineSeriesX.setName("X");
    chart.addSeries(&lineSeriesX);

    lineSeriesY.setPen(QPen(QColor(Qt::black)));
    lineSeriesY << QPointF(0, 0) << QPointF(0, chart.maximumWidth());
    lineSeriesY.setName("Y");

    chart.addSeries(&lineSeriesY);
}

void CentralWidget::createMenu()
{
    menuFile.setTitle("File");
    menuFile.addAction(QIcon(":/save_as"), "save as BMP", this, SLOT(slotSaveBMP()));

    menuView.setTitle("View");
    menuView.addAction(QIcon(":/reset"), "Reset zoom and position(Esp)", this, SLOT(slotResetZoomAndPosition()));

    this->menuBar()->addMenu(&menuFile);
    this->menuBar()->addMenu(&menuView);
}

double CentralWidget::findMaxX(QXYSeries *series)
{
    double maxX = 0;

    foreach (QPointF point, series->points())
        if(point.x() > maxX) maxX = point.x();

    return maxX;
}

double CentralWidget::findMinX(QXYSeries *series)
{
    double minX = std::numeric_limits<double>::max();

    foreach (QPointF point, series->points())
        if(point.x() < minX) minX = point.x();

    return minX;
}

double CentralWidget::findMaxY(QXYSeries *series)
{
    double maxY = 0;

    foreach (QPointF point, series->points())
        if(point.y() > maxY) maxY = point.y();

    return maxY;
}

double CentralWidget::findMinY(QXYSeries *series)
{
    double minY = std::numeric_limits<double>::max();

    foreach (QPointF point, series->points())
        if(point.y() < minY) minY = point.y();

    return minY;
}

void CentralWidget::connectPanelWidgetSignals()
{
    connect(&panelWidget, &PanelWidget::signalChangeTheme, this, &CentralWidget::slotSetTheme);
    connect(&panelWidget, &PanelWidget::signalSetLegendPosition, this, &CentralWidget::slotSetLegentPosition);
    connect(&panelWidget, &PanelWidget::signalSetTitile, this, &CentralWidget::slotSetTitle);
    connect(&panelWidget, &PanelWidget::signalAnimation, this, &CentralWidget::slotAnimation);
    connect(&panelWidget, &PanelWidget::signalAntialiasing, this, &CentralWidget::slotAntialiasing);
    connect(&panelWidget, &PanelWidget::signalTickCountChangeX, this, &CentralWidget::slotSetTcickCountX);
    connect(&panelWidget, &PanelWidget::signalTickCountChangeY, this, &CentralWidget::slotSetTcickCountY);
    connect(&panelWidget, &PanelWidget::signalSeriesDeleted, this, &CentralWidget::slotReRange);
    connect(&panelWidget, &PanelWidget::signalRubberMode, this, &CentralWidget::slotSetRubberMode);
    connect(&panelWidget, &PanelWidget::signalAxisXRangeChanged, this, &CentralWidget::slotRangeXSet);
    connect(&panelWidget, &PanelWidget::signalAxisYRangeChanged, this, &CentralWidget::slotRangeYSet);
    connect(&panelWidget, &PanelWidget::signalSeriesTypeChange, this, &CentralWidget::slotSeriesTypeChanged);
}

QXYSeries *CentralWidget::createSeriesAccordingType(QAbstractSeries::SeriesType &type)
{
    QXYSeries *series = nullptr;
    switch (type)
    {
    case QXYSeries::SeriesTypeLine:
        series = new QLineSeries();
        break;
    case QXYSeries::SeriesTypeSpline:
        series = new QSplineSeries();
        break;
    case QXYSeries::SeriesTypeScatter:
        series = new QScatterSeries();
        break;
    default:
        series = new QLineSeries();
        break;
    }
    return series;
}

void CentralWidget::fillSeriesOfPoints(std::vector<QPointF> &pointVector, QXYSeries *series)
{
    for(auto &point : pointVector)
    {
        *series << point;
        setMinAndMaxForXY(point);
    }
}

void CentralWidget::setMinAndMaxForXY(QPointF &point)
{
    if(point.x() > maxX) maxX = point.x();
    if(point.y() > maxY) maxY = point.y();
    if(point.x() < minX) minX = point.x();
    if(point.y() < minY) minY = point.y();
}

void CentralWidget::setRangeAndTitleForAxes(const QString &axsisX_Title, const QString &axsisY_Title)
{
    chart.axes(Qt::Horizontal).back()->setRange(minX, maxX);
    chart.axes(Qt::Horizontal).back()->setTitleText(axsisX_Title);
    chart.axes(Qt::Vertical).back()->setRange(minY, maxY);
    chart.axes(Qt::Vertical).back()->setTitleText(axsisY_Title);
}

void CentralWidget::setChartViewXYRange()
{
    //Что-бы правильно работало нажатие Esc >>
    chartView.rangeX.min = minX;
    chartView.rangeX.max = maxX;
    chartView.rangeY.min = minY;
    chartView.rangeY.max = maxY;
    //Что-бы правильно работало нажатие Esc <<
}

void CentralWidget::setSeriesProperty(QXYSeries *series)
{
    if(chart.series().length() > 2) //1 - axis x, 2 - axis y
    {
        QXYSeries* s = static_cast<QXYSeries*>(chart.series().last());
        QColor newColor = series->color();
        QPen pen = s->pen();
        pen.setColor(newColor);
        series->setPen(pen);
        if(series->type() == QAbstractSeries::SeriesTypeScatter)
        {
            QScatterSeries* scatSer = static_cast<QScatterSeries*>(series);
            scatSer->setMarkerSize(pen.width());
            scatSer->setBorderColor(newColor);
        }
    }
}

void CentralWidget::createAxes()
{
    chart.createDefaultAxes();

    pAxisX = qobject_cast<QValueAxis*>(chart.axes(Qt::Horizontal).back());
    pAxisY = qobject_cast<QValueAxis*>(chart.axes(Qt::Vertical).back());

    connect(pAxisX, SIGNAL(rangeChanged(qreal, qreal)), this, SLOT(slotRangeXChanged(qreal, qreal)));
    connect(pAxisY, SIGNAL(rangeChanged(qreal, qreal)), this, SLOT(slotRangeYChanged(qreal, qreal)));
}

void CentralWidget::slotSetTheme(int theme)
{
    chartView.chart()->setTheme(QChart::ChartTheme(theme));
    lineSeriesX.setPen(QPen(QColor(Qt::black)));
    lineSeriesY.setPen(QPen(QColor(Qt::black)));
    chart.setDropShadowEnabled(true);
}

void CentralWidget::slotSetLegentPosition(int position)
{
    switch (position) {
    case 0:
        chart.legend()->setAlignment(Qt::AlignTop);
        break;
    case 1:
        chart.legend()->setAlignment(Qt::AlignRight);
        break;
    case 2:
        chart.legend()->setAlignment(Qt::AlignBottom);
        break;
    case 3:
        chart.legend()->setAlignment(Qt::AlignLeft);
        break;
    default:
        break;
    }
}

void CentralWidget::slotSetTitle(QString title)
{
    chart.setTitle(title);
}

void CentralWidget::slotAnimation(bool value)
{
    if(value)
        chart.setAnimationOptions(QChart::AllAnimations);
    else
        chart.setAnimationOptions(QChart::NoAnimation);
}

void CentralWidget::slotAntialiasing(bool value)
{
    if(value)
        chartView.setRenderHint(QPainter::HighQualityAntialiasing);
    else
        chartView.setRenderHint(QPainter::HighQualityAntialiasing, false);
}

void CentralWidget::slotSetTcickCountY(int value)
{
    QValueAxis* axis = static_cast<QValueAxis*>(chart.axes(Qt::Vertical).back());
    axis->setTickCount(value);
}

void CentralWidget::slotSaveBMP()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    ("Save as BMP"),
                                                    QDir::rootPath() + chart.title(),
                                                    "Images (*.bmp);;All files (*.*)");
    if(fileName != "")
    {
        QPixmap pix = chartView.grab();
        pix.toImage().save(fileName, "BMP");
    }
}

void CentralWidget::slotReRange()
{
    maxX = 0;
    minX = std::numeric_limits<double>::max();
    maxY = 0;
    minY = std::numeric_limits<double>::max();

    for (int i = 2; i < chart.series().count(); ++i)
    {
        QXYSeries* series = static_cast<QXYSeries*>(chart.series().at(i));
        if(maxX < findMaxX(series)) maxX = findMaxX(series);
        if(minX > findMinX(series)) minX = findMinX(series);
        if(maxY < findMaxY(series)) maxY = findMaxY(series);
        if(minY > findMinY(series)) minY = findMinY(series);
    }
    //Что-бы правильно работало нажатие Esc >>
    chartView.rangeX.min = minX;
    chartView.rangeX.max = maxX;

    chartView.rangeY.min = minY;
    chartView.rangeY.max = maxY;
    //Что-бы правильно работало нажатие Esc <<

    QString axisX_Title = chart.axes(Qt::Horizontal).back()->titleText();
    QString axisY_Title = chart.axes(Qt::Vertical).back()->titleText();

    createAxes();
    chart.axes(Qt::Horizontal).back()->setRange(minX, maxX);
    chart.axes(Qt::Horizontal).back()->setTitleText(axisX_Title);
    chart.axes(Qt::Vertical).back()->setRange(minY, maxY);
    chart.axes(Qt::Vertical).back()->setTitleText(axisY_Title);
}

void CentralWidget::slotResetZoomAndPosition()
{
    chartView.slotResetZoomAndPosition();
}

void CentralWidget::slotViewXYCoordinate(QPointF point)
{
    statusBarWidget.setText("X: " + QString::number(point.x()) +
                            "   Y: " + QString::number(point.y()));
}

void CentralWidget::slotSetRubberMode(QChartView::RubberBand mode)
{
    chartView.setRubberBand(mode);
}

void CentralWidget::slotRangeXChanged(qreal min, qreal max)
{
    panelWidget.setRangeAxisX(min, max);
}

void CentralWidget::slotRangeYChanged(qreal min, qreal max)
{
    panelWidget.setRangeAxisY(min, max);
}

void CentralWidget::slotRangeXSet(qreal min, qreal max)
{
    pAxisX->setRange(min, max);
}

void CentralWidget::slotRangeYSet(qreal min, qreal max)
{
    pAxisY->setRange(min, max);
}

void CentralWidget::slotSeriesTypeChanged()
{
    QString axisX_Title = chart.axes(Qt::Horizontal).back()->titleText();
    QString axisY_Title = chart.axes(Qt::Vertical).back()->titleText();

    createAxes();

    chart.axes(Qt::Horizontal).back()->setTitleText(axisX_Title);
    chart.axes(Qt::Vertical).back()->setTitleText(axisY_Title);
}

void CentralWidget::closeEvent(QCloseEvent*)
{
    emit signalCloseWindow(this);
}

void CentralWidget::slotSetTcickCountX(int value)
{
    QValueAxis* axis = static_cast<QValueAxis*>(chart.axes(Qt::Horizontal).back());
    axis->setTickCount(value);
}
