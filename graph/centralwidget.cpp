#include "centralwidget.h"

#include <QPushButton>
#include <QFileDialog>
#include <QFileInfo>
#include <QDebug>
#include <QStatusBar>
#include <limits>

#include <QApplication>

#define FONT_SIZE 18

CentralWidget::CentralWidget(QWidget *parent) : QMainWindow(parent),
    _pAxisX(nullptr), _pAxisY(nullptr)
{
    _panelWidget.setParent(this);
    createMenu();
    statusBar()->insertWidget(0, &_statusBarWidget);
    setLinersXYDefault();

    QFont font = _chart.titleFont();
    font.setPixelSize(FONT_SIZE);
    _chart.setTitleFont(QFont(font));

    createAxes();
    _pAxisX->setTitleText("X");
    _chart.axes(Qt::Vertical).first()->setTitleText("Y");
    _chart.setDropShadowEnabled(true);

    _chartView.setChart(&_chart);

    _layout.addWidget(&_chartView);
    _layout.addWidget(&_panelWidget);
    _layout.setStretch(0, 1);

    _centralWidget.setLayout(&_layout);
    setCentralWidget(&_centralWidget);
    _chartView.setFocus();

    connectPanelWidgetSignals();
    connect(&_chartView, &ChartView::signalMousePosition, this, &CentralWidget::slotViewXYCoordinate);
}

CentralWidget::~CentralWidget()
{
    delete _pAxisX;
    delete _pAxisY;
}

void CentralWidget::addSeries(std::map<double, double> &map, QXYSeries::SeriesType type, QString legendTitle, QString axsisX_Title, QString axsisY_Title)
{
    QXYSeries* series = createSeriesAccordingType(type);
    series->setName(legendTitle);

    fillSeriesOfPoints(map, series);
    setSeriesProperty(series);
    setChartViewXYRange();

    _chart.addSeries(series);
    createAxes();
    setRangeAndTitleForAxes(axsisX_Title, axsisY_Title);

    _panelWidget.addSeriesInList(series);
}

void CentralWidget::addSeries(std::map<double, double> &map, QString legendTitle, QString axsisX_Title, QString axsisY_Title)
{
    addSeries(map, QXYSeries::SeriesType(_panelWidget.getSeriesType()), legendTitle, axsisX_Title, axsisY_Title);
}

void CentralWidget::setTitle(QString title)
{
    _chart.setTitle(title);
    _panelWidget.setTitle(_chart.title());
}

QString CentralWidget::getTitle() const
{
    return _chart.title();
}

QString CentralWidget::getDataXType() const
{
    return _chart.axes(Qt::Horizontal).first()->titleText();
}
void CentralWidget::setLinersXYDefault()
{
    _lineSeriesX.setPen(QPen(QColor(Qt::black)));
    _lineSeriesX << QPointF(0, 0) << QPointF(/*_chart.maximumWidth()*/1000, 0);
    _lineSeriesX.setName("X");
    _chart.addSeries(&_lineSeriesX);

    _lineSeriesY.setPen(QPen(QColor(Qt::black)));
    _lineSeriesY << QPointF(0, 0) << QPointF(0, /*_chart.maximumWidth()*/1000);
    _lineSeriesY.setName("Y");

    _chart.addSeries(&_lineSeriesY);
}

void CentralWidget::createMenu()
{
    _menuFile.setTitle("File");
    _menuFile.addAction(QIcon(":/save_as"), "save as BMP", this, SLOT(slotSaveBMP()));

    _menuView.setTitle("View");
    _menuView.addAction(QIcon(":/reset"), "Reset zoom and position(Esp)", this, SLOT(slotResetZoomAndPosition()));

    this->menuBar()->addMenu(&_menuFile);
    this->menuBar()->addMenu(&_menuView);
}

double CentralWidget::findMaxX(QXYSeries *series)
{
    double maxX = std::numeric_limits<double>::min();

    for (auto point : series->points())
        if(point.x() > maxX)
            maxX = point.x();

    return maxX;
}

double CentralWidget::findMinX(QXYSeries *series)
{
    double minX = std::numeric_limits<double>::max();

    for (auto point : series->points())
        if(point.x() < minX)
            minX = point.x();

    return minX;
}

double CentralWidget::findMaxY(QXYSeries *series)
{
    double maxY = std::numeric_limits<double>::min();

    for (auto point : series->points())
        if(point.y() > maxY)
            maxY = point.y();

    return maxY;
}

double CentralWidget::findMinY(QXYSeries *series)
{
    double minY = std::numeric_limits<double>::max();

    for (auto point : series->points())
        if(point.y() < minY)
            minY = point.y();

    return minY;
}

void CentralWidget::connectPanelWidgetSignals()
{
    connect(&_panelWidget, &PanelWidget::signalChangeTheme, this, &CentralWidget::slotSetTheme);
    connect(&_panelWidget, &PanelWidget::signalSetLegendPosition, this, &CentralWidget::slotSetLegentPosition);
    connect(&_panelWidget, &PanelWidget::signalSetTitile, this, &CentralWidget::slotSetTitle);
    connect(&_panelWidget, &PanelWidget::signalAnimation, this, &CentralWidget::slotAnimation);
    connect(&_panelWidget, &PanelWidget::signalAntialiasing, this, &CentralWidget::slotAntialiasing);
    connect(&_panelWidget, &PanelWidget::signalTickCountChangeX, this, &CentralWidget::slotSetTcickCountX);
    connect(&_panelWidget, &PanelWidget::signalTickCountChangeY, this, &CentralWidget::slotSetTcickCountY);
    connect(&_panelWidget, &PanelWidget::signalSeriesDeleted, this, &CentralWidget::slotReRange);
    connect(&_panelWidget, &PanelWidget::signalRubberMode, this, &CentralWidget::slotSetRubberMode);
    connect(&_panelWidget, &PanelWidget::signalAxisXRangeChanged, this, &CentralWidget::slotRangeXSet);
    connect(&_panelWidget, &PanelWidget::signalAxisYRangeChanged, this, &CentralWidget::slotRangeYSet);
    connect(&_panelWidget, &PanelWidget::signalSeriesTypeChanged, this, &CentralWidget::slotSeriesTypeChanged);
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

void CentralWidget::fillSeriesOfPoints(std::map<double, double> &map, QXYSeries *series)
{
    for(auto [key, value] : map)
        *series << QPointF(key, value);

    setMinAndMaxForXY(series);
}

void CentralWidget::setMinAndMaxForXY(QXYSeries* series)
{
    double maxX = findMaxX(series);
    double minX = findMinX(series);

    double maxY = findMaxY(series);
    double minY = findMinY(series);

    if(_rangeX.max() < maxX)
        _rangeX.setMax(maxX);
    if(_rangeX.min() > minX)
        _rangeX.setMin(minX);

    if(_rangeY.max() < maxY)
        _rangeY.setMax(maxY);
    if(_rangeY.min() > minY)
        _rangeY.setMin(minY);

    repaintXYAxes();
}

void CentralWidget::setRangeAndTitleForAxes(const QString &axsisX_Title, const QString &axsisY_Title)
{
    _chart.axes(Qt::Horizontal).first()->setRange(_rangeX.min(), _rangeX.max());
    _chart.axes(Qt::Horizontal).first()->setTitleText(axsisX_Title);
    _chart.axes(Qt::Vertical).first()->setRange(_rangeY.min(), _rangeY.max());
    _chart.axes(Qt::Vertical).first()->setTitleText(axsisY_Title);
}

void CentralWidget::setChartViewXYRange()
{
    _chartView.setRangeDefault(_rangeX, _rangeY);
}

void CentralWidget::setSeriesProperty(QXYSeries *series)
{
    if(_chart.series().length() <= 2)
        return;

    QXYSeries* s = static_cast<QXYSeries*>(_chart.series().last());

    QColor newColor = QColor(std::rand() % (255 + 1 - 0) + 0,
                             std::rand() % (255 + 1 - 0) + 0,
                             std::rand() % (255 + 1 - 0) + 0);
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

void CentralWidget::repaintXYAxes()
{
    if(_rangeX.max() < 1)
        _rangeX.setMax(1);
    if(_rangeY.max() < 1)
        _rangeY.setMax(1);

    _lineSeriesX.clear();
    _lineSeriesX << QPointF(0, 0) << QPointF(_rangeX.max(), 0);
    _lineSeriesY.clear();
    _lineSeriesY << QPointF(0, 0) << QPointF(0, _rangeY.max());
}

void CentralWidget::createAxes()
{
    _chart.createDefaultAxes();

    _pAxisX = qobject_cast<QValueAxis*>(_chart.axes(Qt::Horizontal).first());
    _pAxisY = qobject_cast<QValueAxis*>(_chart.axes(Qt::Vertical).first());

    connect(_pAxisX, SIGNAL(rangeChanged(qreal, qreal)), this, SLOT(slotRangeXChanged(qreal, qreal)));
    connect(_pAxisY, SIGNAL(rangeChanged(qreal, qreal)), this, SLOT(slotRangeYChanged(qreal, qreal)));
}

void CentralWidget::slotSetTheme(int theme)
{
    _chartView.chart()->setTheme(QChart::ChartTheme(theme));
    _lineSeriesX.setPen(QPen(QColor(Qt::black)));
    _lineSeriesY.setPen(QPen(QColor(Qt::black)));
    _chart.setDropShadowEnabled(true);
}

void CentralWidget::slotSetLegentPosition(int position)
{
    switch (position) {
    case 0:
        _chart.legend()->setAlignment(Qt::AlignTop);
        break;
    case 1:
        _chart.legend()->setAlignment(Qt::AlignRight);
        break;
    case 2:
        _chart.legend()->setAlignment(Qt::AlignBottom);
        break;
    case 3:
        _chart.legend()->setAlignment(Qt::AlignLeft);
        break;
    default:
        break;
    }
}

void CentralWidget::slotSetTitle(QString title)
{
    _chart.setTitle(title);
}

void CentralWidget::slotAnimation(bool value)
{
    if(value)
        _chart.setAnimationOptions(QChart::AllAnimations);
    else
        _chart.setAnimationOptions(QChart::NoAnimation);
}

void CentralWidget::slotAntialiasing(bool value)
{
    if(value)
        _chartView.setRenderHint(QPainter::HighQualityAntialiasing);
    else
        _chartView.setRenderHint(QPainter::HighQualityAntialiasing, false);
}

void CentralWidget::slotSetTcickCountY(int value)
{
    QValueAxis* axis = static_cast<QValueAxis*>(_chart.axes(Qt::Vertical).first());
    axis->setTickCount(value);
}

void CentralWidget::slotSaveBMP()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    ("Save as BMP"),
                                                    QDir::rootPath() + _chart.title(),
                                                    "Images (*.bmp);;All files (*.*)");
    if(fileName != "")
    {
        QPixmap pix = _chartView.grab();
        pix.toImage().save(fileName, "BMP");
    }
}

void CentralWidget::slotReRange()
{
    _rangeX.setMax(std::numeric_limits<double>::min());
    _rangeX.setMin(std::numeric_limits<double>::max());
    _rangeY.setMax(std::numeric_limits<double>::min());
    _rangeY.setMin(std::numeric_limits<double>::max());

    for (int i = 2; i < _chart.series().count(); ++i)
    {
        QXYSeries* series = static_cast<QXYSeries*>(_chart.series().at(i));
        setMinAndMaxForXY(series);
    }
    _chartView.setRangeDefault(_rangeX, _rangeY);

    QString axisX_Title = _chart.axes(Qt::Horizontal).first()->titleText();
    QString axisY_Title = _chart.axes(Qt::Vertical).first()->titleText();

    createAxes();
    _chart.axes(Qt::Horizontal).first()->setRange(_rangeX.min(), _rangeX.max());
    _chart.axes(Qt::Horizontal).first()->setTitleText(axisX_Title);
    _chart.axes(Qt::Vertical).first()->setRange(_rangeY.min(), _rangeY.max());
    _chart.axes(Qt::Vertical).first()->setTitleText(axisY_Title);
}

void CentralWidget::slotResetZoomAndPosition()
{
    _chartView.slotResetZoomAndPosition();
}

void CentralWidget::slotViewXYCoordinate(QPointF point)
{
    _statusBarWidget.setText("X: " + QString::number(point.x()) +
                            "   Y: " + QString::number(point.y()));
}

void CentralWidget::slotSetRubberMode(QChartView::RubberBand mode)
{
    _chartView.setRubberBand(mode);
}

void CentralWidget::slotRangeXChanged(qreal min, qreal max)
{
    _panelWidget.setRangeAxisX(min, max);
}

void CentralWidget::slotRangeYChanged(qreal min, qreal max)
{
    _panelWidget.setRangeAxisY(min, max);
}

void CentralWidget::slotRangeXSet(qreal min, qreal max)
{
    _pAxisX->setRange(min, max);
}

void CentralWidget::slotRangeYSet(qreal min, qreal max)
{
    _pAxisY->setRange(min, max);
}

void CentralWidget::slotSeriesTypeChanged()
{
    QString axisX_Title = _chart.axes(Qt::Horizontal).first()->titleText();
    QString axisY_Title = _chart.axes(Qt::Vertical).first()->titleText();

    createAxes();

    _chart.axes(Qt::Horizontal).first()->setTitleText(axisX_Title);
    _chart.axes(Qt::Vertical).first()->setTitleText(axisY_Title);
}

void CentralWidget::closeEvent(QCloseEvent*)
{
    emit signalCloseWindow(this);
}

void CentralWidget::slotSetTcickCountX(int value)
{
    QValueAxis* axis = static_cast<QValueAxis*>(_chart.axes(Qt::Horizontal).first());
    axis->setTickCount(value);
}
