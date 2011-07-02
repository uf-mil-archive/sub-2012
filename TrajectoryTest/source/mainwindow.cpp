#include "TrajectoryTest/mainwindow.h"
#include "DataObjects/Waypoint/Waypoint.h"

#include <qwt_plot_grid.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_directpainter.h>
#include <qpen.h>
#include <QDebug>

using namespace subjugator;
using namespace Eigen;

QVector<LocalWaypointDriverDynamicInfo> poseList;

static points update_pos(int index)
{
    //return poseList.data()[index];
	points pos;
	//pos.x = poseList.data()[index].CurrentTrajectory->DesiredTrajectory(0,0);
	pos.y = index;

	return pos;
}

//static double wave(double x)
//{
//    const double period = 1.0;
//    const double c = 5.0;
//
//    double v = ::fmod(x, period);
//
//    const double amplitude = qAbs(x - qRound(x / c) * c) / ( 0.5 * c );
//    v = amplitude * qSin(v / period * 2 * M_PI);
//
//    return v;
//}
//
//static double noise(double)
//{
//    return 2.0 * ( qrand() / ((double)RAND_MAX + 1) ) - 1.0;
//}
//
//static double ramp(double x)
//{
//    return x;
//}

class FunctionData: public QwtSyntheticPointData
{
public:
    FunctionData(points(*y)(int)):
        QwtSyntheticPointData(100),
        d_y(y)
    {
    }

    virtual points y(int x) const
    {
        return d_y(x);
    }

private:
    points(*d_y)(int);
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    d_paintedPoints(0),
    d_interval(-20.0, 20.0),
    d_timerId(-1),
	numOfPoints(500)
{
    int updateInterval = 20;

    // FOR TESTING TRAJECTORYGENERATOR
    Vector6d trajStartPoint = Vector6d::Zero();

    //Waypoint *waypt = new Waypoint();

    trajectoryGenerator = new TrajectoryGenerator(trajStartPoint);
    trajectoryGenerator->SetWaypoint(*new Waypoint(), true);

    ui->setupUi(this);
    d_timerId = startTimer(updateInterval);

    curvePlot1  = new QwtPlotCurve();
    curvePlot2  = new QwtPlotCurve();

    setupPlot(ui->qwtPlot1);
    setupPlot(ui->qwtPlot2);
    setupCurve(curvePlot1, QPen(Qt::cyan));
    setupCurve(curvePlot2, QPen(Qt::green));

    curvePlot1->setData(new DataSeries(20.0, 10));

    DataSeries *buffer = (DataSeries *)curvePlot1->data();
    buffer->setFunction(update_pos);
    buffer->fill(20.0, 1000);

    //curvePlot1->setData(new FunctionData(::cos));
    curvePlot1->attach(ui->qwtPlot1);

    //curvePlot2->setData(new FunctionData(::sin));
    curvePlot2->attach(ui->qwtPlot2);

    d_clock.start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//void MainWindow::addPoint(points pos)
//{
//    poseList.append(pos);
//
//    if (poseList.size() > numOfPoints)
//        poseList.remove(1,1);
//}

void MainWindow::addPoint(LocalWaypointDriverDynamicInfo p)
{
    poseList.append(p);

    if (poseList.size() > numOfPoints)
        poseList.remove(1,1);
}

// Function to setup plots with like values.
void MainWindow::setupPlot(QwtPlot *plot)
{
    // Create and setup Plot
    d_directPainter = new QwtPlotDirectPainter();

    plot->setAutoReplot(false);
    plot->canvas()->setPaintAttribute(QwtPlotCanvas::BackingStore, false);

    initGradient(plot);

    plot->plotLayout()->setAlignCanvasToScales(true);
    plot->setAxisTitle(QwtPlot::xBottom, "Time [s]");
    plot->setAxisScale(QwtPlot::xBottom, d_interval.minValue(), d_interval.maxValue());
    plot->setAxisScale(QwtPlot::yLeft, -1.0, 1.0);

    // Create Grid within plot
    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->setPen(QPen(Qt::gray, 0.0, Qt::DotLine));
    grid->enableX(true);
    grid->enableXMin(true);
    grid->enableY(true);
    grid->enableYMin(false);
    grid->attach(plot);

    // Setup Plot Markers
    QwtPlotMarker *d_origin = new QwtPlotMarker();
    d_origin->setLineStyle(QwtPlotMarker::Cross);
    d_origin->setValue(d_interval.minValue() + d_interval.width() / 2.0, 0.0);
    d_origin->setLinePen(QPen(Qt::gray, 0.0, Qt::DashLine));
    d_origin->attach(plot);
}

// Function to setup curves with default values to use with plots.
void MainWindow::setupCurve(QwtPlotCurve *curve, QPen pen)
{
    // Create and setup Curves
    curve->setStyle(QwtPlotCurve::Lines);
    curve->setPen(pen);
#if 1
    curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
#endif
#if 1
    curve->setPaintAttribute(QwtPlotCurve::ClipPolygons, false);
#endif
}

// Function to create gradient for plot backgrounds.
void MainWindow::initGradient(QwtPlot *plot)
{
    QPalette pal = plot->canvas()->palette();

#if QT_VERSION >= 0x040400
    QLinearGradient gradient( 0.0, 0.0, 1.0, 0.0 );
    gradient.setCoordinateMode( QGradient::StretchToDeviceMode );
    gradient.setColorAt(0.0, QColor( 0, 49, 110 ) );
    gradient.setColorAt(1.0, QColor( 0, 87, 174 ) );

    pal.setBrush(QPalette::Window, QBrush(gradient));
#else
    pal.setBrush(QPalette::Window, QBrush( color ));
#endif

    plot->canvas()->setPalette(pal);
}

void MainWindow::start()
{

}

void MainWindow::on_actionRPY_triggered()
{
    //curvePlot1->setData(new FunctionData(::sin));
    curvePlot1->attach(ui->qwtPlot1);

    //curvePlot2->setData(new FunctionData(::cos));
    curvePlot2->attach(ui->qwtPlot1);

    ui->qwtPlot1->replot();
    ui->qwtPlot2->replot();
}

void MainWindow::on_actionPOS_triggered()
{
    //curvePlot1->setData(new FunctionData(::sin));
    curvePlot1->attach(ui->qwtPlot2);

    //curvePlot2->setData(new FunctionData(::cos));
    curvePlot2->attach(ui->qwtPlot2);

    ui->qwtPlot1->replot();
    ui->qwtPlot2->replot();
}

void MainWindow::on_actionError_triggered()
{
    ui->qwtPlot1->replot();
    ui->qwtPlot2->replot();
}

// ADDED FOR SIMULATION
void MainWindow::timerEvent(QTimerEvent *)
{
	Vector6d wrench = Vector6d::Zero();
	trajectoryGenerator->Update(getTimestamp());

	LocalWaypointDriverDynamicInfo info;
//	info.RequestedWrench = wrench;
//	info.CurrentTrajectory = trajectoryGenerator->ReportDynamicInfo();

//	LocalWaypointDriverDynamicInfo* info = new LocalWaypointDriverDynamicInfo(wrench, &trajectoryGenerator->ReportDynamicInfo());

	//addPoint(*info);

    DataSeries *buffer = (DataSeries *)curvePlot1->data();
    buffer->setReferenceTime(d_clock.elapsed() / 1000.0);

    ui->qwtPlot1->replot();
}

boost::int64_t MainWindow::getTimestamp(void)
{
	timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);

	return ((long long int)t.tv_sec * NSEC_PER_SEC) + t.tv_nsec;
}
