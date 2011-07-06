// X is (lpos.position_NED, RPY from lpos.getQuat_NED_B)  Actual Position
// Xd is (xyzrpy from getTrajectory) Desired Position
// x_dot is Actual Velocity and Angular
// xd_dot is Desired Velocity

#include "TrajectoryTest/mainwindow.h"
#include "DataObjects/Waypoint/Waypoint.h"

#include <qwt_plot_grid.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_legend.h>
#include <qwt_plot_directpainter.h>
#include <qpen.h>
#include <QDebug>

using namespace subjugator;
using namespace std;
using namespace Eigen;

QVector<LocalWaypointDriverInfo> poseList;
QVector<double> testPts;

// Plot Actual Position.
static points PlotActualPosition(int index, char c)
{
	points pos;

	pos.x = index;

	if (c == 'x')
		pos.y = (poseList[index].X(0));
	else if (c == 'y')
		pos.y = (poseList[index].X(1));
	else if (c == 'z')
		pos.y = (poseList[index].X(2));
	else
		pos.y = 0;

	return pos;
}

// Plot Desired Position.
static points PlotDesiredPosition(int index, char c)
{
	points pos;

	pos.x = index;

	if (c == 'x')
		pos.y = (poseList[index].Xd(0));
	else if (c == 'y')
		pos.y = (poseList[index].Xd(1));
	else if (c == 'z')
		pos.y = (poseList[index].Xd(2));
	else
		pos.y = 0;

	return pos;
}

// Plot Actual Velocity
static points PlotActualVelocity(int index, char c)
{
	points pos;

	pos.x = index;

	if (c == 'x')
		pos.y = (poseList[index].X_dot(0));
	else if (c == 'y')
		pos.y = (poseList[index].X_dot(1));
	else if (c == 'z')
		pos.y = (poseList[index].X_dot(2));
	else
		pos.y = 0;

	return pos;
}


// Plot Desired Velocity
static points PlotDesiredVelocity(int index, char c)
{
	points pos;

	pos.x = index;

	if (c == 'x')
		pos.y = (poseList[index].Xd_dot(0));
	else if (c == 'y')
		pos.y = (poseList[index].Xd_dot(1));
	else if (c == 'z')
		pos.y = (poseList[index].Xd_dot(2));
	else
		pos.y = 0;

	return pos;
}

// Plot Actual RPY
static points PlotActualRPY(int index, char c)
{
	points pos;

	pos.x = index;
	if(c == 'p')
		pos.y = (180.0/M_PI)*(poseList[index].X(4));
	else if(c == 'y')
		pos.y = (180.0/M_PI)*(poseList[index].X(5));
	else
		pos.y = 10;

	return pos;
}

// Plot Desired RPY
static points PlotDesiredRPY(int index, char c)
{
	points pos;

	pos.x = index;
	if(c == 'p')
		pos.y = (180.0/M_PI)*(poseList[index].Xd(4));
	else if(c == 'y')
		pos.y = (180.0/M_PI)*(poseList[index].Xd(5));
	else
		pos.y = 10;

	return pos;
}

// Plot Actual Angular Velocity
static points PlotActualAngularVelocity(int index, char c)
{
	points pos;

	pos.x = index;
	if(c == 'p')
		pos.y = (180.0/M_PI)*(poseList[index].X_dot(4));
	else if(c == 'y')
		pos.y = (180.0/M_PI)*(poseList[index].X_dot(5));
	else
		pos.y = 10;

	return pos;
}

// Plot Desired Angular Velocity
static points PlotDesiredAngularVelocity(int index, char c)
{
	points pos;

	pos.x = index;
	if(c == 'p')
		pos.y = (180.0/M_PI)*(poseList[index].Xd_dot(4));
	else if(c == 'y')
		pos.y = (180.0/M_PI)*(poseList[index].Xd_dot(5));
	else
		pos.y = 10;

	return pos;
}

// Plot Position Error
static points PlotPositionError(int index, char c)
{
	points pos;

	pos.x = index;

	if (c == 'x')
		pos.y = (poseList[index].X(0) - poseList[index].Xd(0));
	else if (c == 'y')
		pos.y = (poseList[index].X(1) - poseList[index].Xd(1));
	else if (c == 'z')
		pos.y = (poseList[index].X(2) - poseList[index].Xd(2));
	else
		pos.y = 0;

	return pos;
}

// Plot RPY Error
static points PlotRPYError(int index, char c)
{
	points pos;

	pos.x = index;

	if (c == 'p')
		pos.y = (180.0/M_PI)*(poseList[index].X(4) - poseList[index].Xd(4));
	else if (c == 'y')
		pos.y = (180.0/M_PI)*(poseList[index].X(5) - poseList[index].Xd(5));
	else
		pos.y = 0;

	return pos;
}

// Plot Velocity Error
static points PlotVelocityError(int index, char c)
{
	points pos;

	pos.x = index;

	if (c == 'x')
		pos.y = (poseList[index].X_dot(0) - poseList[index].Xd_dot(0));
	else if (c == 'y')
		pos.y = (poseList[index].X_dot(1) - poseList[index].Xd_dot(1));
	else if (c == 'z')
		pos.y = (poseList[index].X_dot(2) - poseList[index].Xd_dot(2));
	else
		pos.y = 0;

	return pos;
}

// Plot Angular Velocity Error
static points PlotAngularVelocityError(int index, char c)
{
	points pos;

	pos.x = index;

	if (c == 'p')
		pos.y = (180.0/M_PI)*(poseList[index].X_dot(4) - poseList[index].Xd_dot(4));
	else if (c == 'y')
		pos.y = (180.0/M_PI)*(poseList[index].X_dot(5) - poseList[index].Xd_dot(5));
	else
		pos.y = 0;

	return pos;
}

// Class for building Synthetic Point Data for QWT Plots
class FunctionData: public QwtSyntheticPointData
{
public:
    FunctionData(points(*y)(int, char)):
        QwtSyntheticPointData(500),
        d_y(y)
    {
    }

    virtual points y(int x, char c) const
    {
        return d_y(x,c);
    }

private:
    points(*d_y)(int, char);
};

MainWindow::MainWindow(DDSDomainParticipant *participant, DDSDomainParticipant *partSender, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    d_paintedPoints(0),
    d_interval(0, 500),
    d_timerId(-1),
	numOfPoints(500),
	updateInterval(20), // ms
	updateToggle(false),
	posPlot(true),
	rpyPlot(false),
	errposPlot(false),
	errrpyPlot(false),
	compare1Plot(false),
	compare2Plot(false),
	actualToggle(false),
	testToggle(false),
	trajectoryreceiver(participant, "Trajectory", bind(&MainWindow::DDSReadCallback, this, _1)),
	waypointddssender(partSender, "SetWaypoint"),
	gainsddssender(partSender, "ControllerGains")
{
    poseList.resize(numOfPoints);

    ui->setupUi(this);
    ui->statusBar->showMessage("Plotting Stopped");

    ui->btnSubmitStart->setVisible(false);
    ui->btnCallUpdate->setVisible(false);

    connect(this, SIGNAL(trajectoryReceived()), this, SLOT(onTrajectoryReceived()));

    curve1_Plot1  = new QwtPlotCurve("Pos X");
    curve2_Plot1  = new QwtPlotCurve("Pos Y");
    curve3_Plot1  = new QwtPlotCurve("Pos Z");
    curve1_Plot2  = new QwtPlotCurve("Vel X");
    curve2_Plot2  = new QwtPlotCurve("Vel Y");
    curve3_Plot2  = new QwtPlotCurve("Vel Z");

    setupPlot(ui->qwtPlot1);
    setupPlot(ui->qwtPlot2);
    setupCurve(curve1_Plot1, QPen(Qt::cyan, 3));
    setupCurve(curve2_Plot1, QPen(Qt::green, 3));
    setupCurve(curve3_Plot1, QPen(Qt::yellow,3));
    setupCurve(curve1_Plot2, QPen(Qt::cyan, 3));
	setupCurve(curve2_Plot2, QPen(Qt::green, 3));
	setupCurve(curve3_Plot2, QPen(Qt::yellow, 3));

    curve1_Plot1->setData(new DataSeries(20.0, numOfPoints));
    curve2_Plot1->setData(new DataSeries(20.0, numOfPoints));
    curve3_Plot1->setData(new DataSeries(20.0, numOfPoints));
    curve1_Plot2->setData(new DataSeries(20.0, numOfPoints));
	curve2_Plot2->setData(new DataSeries(20.0, numOfPoints));
	curve3_Plot2->setData(new DataSeries(20.0, numOfPoints));

	// Set Three curves per plot.
	DataSeries *buffer1 = (DataSeries *)curve1_Plot1->data();
    buffer1->setFunction(PlotDesiredPosition);
    buffer1->setComponent('x');
    buffer1->fill(20.0, numOfPoints);

	DataSeries *buffer2 = (DataSeries *)curve2_Plot1->data();
    buffer2->setFunction(PlotDesiredPosition);
    buffer2->setComponent('y');
    buffer2->fill(20.0, numOfPoints);

	DataSeries *buffer3 = (DataSeries *)curve3_Plot1->data();
    buffer3->setFunction(PlotDesiredPosition);
    buffer3->setComponent('z');
    buffer3->fill(20.0, numOfPoints);

	DataSeries *buffer4 = (DataSeries *)curve1_Plot2->data();
    buffer4->setFunction(PlotDesiredVelocity);
    buffer4->setComponent('x');
    buffer4->fill(20.0, numOfPoints);

	DataSeries *buffer5 = (DataSeries *)curve2_Plot2->data();
    buffer5->setFunction(PlotDesiredVelocity);
    buffer5->setComponent('y');
    buffer5->fill(20.0, numOfPoints);

	DataSeries *buffer6 = (DataSeries *)curve3_Plot2->data();
    buffer6->setFunction(PlotDesiredVelocity);
    buffer6->setComponent('z');
    buffer6->fill(20.0, numOfPoints);

    // Attach curves to their respective plots
    curve1_Plot1->attach(ui->qwtPlot1);
    curve2_Plot1->attach(ui->qwtPlot1);
    curve3_Plot1->attach(ui->qwtPlot1);
    curve1_Plot2->attach(ui->qwtPlot2);
    curve2_Plot2->attach(ui->qwtPlot2);
    curve3_Plot2->attach(ui->qwtPlot2);

    //ui->qwtPlot1->setAxisAutoScale(QwtPlot::yLeft);
	ui->qwtPlot1->setTitle("Desired Position");
	ui->qwtPlot2->setTitle("Actual Position");

    ui->qwtPlot1->replot();
    ui->qwtPlot2->replot();

    d_clock.start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onTrajectoryReceived()
{
	Vector6d x = Vector6d::Zero();
	Vector6d x_dot = Vector6d::Zero();
	Vector6d xd = Vector6d::Zero();
	Vector6d xd_dot = Vector6d::Zero();

	for (int i=0; i<6; i++)
		x(i) = trajectorymsg.x[i];

	for (int i=0; i<6; i++)
			x_dot(i) = trajectorymsg.x_dot[i];

	for (int i=0; i<6; i++)
			xd(i) = trajectorymsg.xd[i];

	for (int i=0; i<6; i++)
			xd_dot(i) = trajectorymsg.xd_dot[i];

	addPoint(LocalWaypointDriverInfo(0,getTimestamp(), Matrix<double, 6, 1>::Zero(), x, x_dot, xd, xd_dot));

	DataSeries *buffer1 = (DataSeries *)curve1_Plot1->data();
	buffer1->update(poseList.size());

	DataSeries *buffer2 = (DataSeries *)curve2_Plot1->data();
	buffer2->update(poseList.size());

	DataSeries *buffer3 = (DataSeries *)curve3_Plot1->data();
	buffer3->update(poseList.size());

	DataSeries *buffer4 = (DataSeries *)curve1_Plot2->data();
	buffer4->update(poseList.size());

	DataSeries *buffer5 = (DataSeries *)curve2_Plot2->data();
	buffer5->update(poseList.size());

	DataSeries *buffer6 = (DataSeries *)curve3_Plot2->data();
	buffer6->update(poseList.size());

	double minValP1 = 0.0, maxValP1 = 0.0, minValP2 = 0.0, maxValP2 = 0.0;

	if (posPlot)
	{
		for (int j = 0; j < poseList.size(); j ++)
		{
			double temp = 0.0;

			temp = (poseList[j].Xd.block<3,1>(0,0)).minCoeff();
			if (temp < minValP1)
				minValP1 = temp;

			temp = (poseList[j].Xd.block<3,1>(0,0)).maxCoeff();
			if (temp > maxValP1)
				maxValP1 = temp;

			temp = (poseList[j].Xd_dot.block<3,1>(0,0)).minCoeff();
			if (temp < minValP2)
				minValP2 = temp;

			temp = (poseList[j].Xd_dot.block<3,1>(0,0)).maxCoeff();
			if (temp > maxValP2)
				maxValP2 = temp;
		}
	}
	else if (rpyPlot)
	{
		for (int j = 0; j < poseList.size(); j ++)
		{
			double temp = 0.0;

			temp = (poseList[j].Xd.block<3,1>(3,0)).minCoeff();
			if (temp < minValP1)
				minValP1 = temp;

			temp = (poseList[j].Xd.block<3,1>(3,0)).maxCoeff();
			if (temp > maxValP1)
				maxValP1 = temp;

			temp = (poseList[j].Xd_dot.block<3,1>(3,0)).minCoeff();
			if (temp < minValP2)
				minValP2 = temp;

			temp = (poseList[j].Xd_dot.block<3,1>(3,0)).maxCoeff();
			if (temp > maxValP2)
				maxValP2 = temp;
		}
	}
	else if (errposPlot)
	{
		for (int j = 0; j < poseList.size(); j ++)
		{
			double temp = 0.0;

			temp = (poseList[j].X.block<3,1>(0,0)-poseList[j].Xd.block<3,1>(0,0)).minCoeff();
			if (temp < minValP1)
				minValP1 = temp;

			temp = (poseList[j].X.block<3,1>(0,0)-poseList[j].Xd.block<3,1>(0,0)).maxCoeff();
			if (temp > maxValP1)
				maxValP1 = temp;

			temp = (poseList[j].X_dot.block<3,1>(0,0)-poseList[j].Xd_dot.block<3,1>(0,0)).minCoeff();
			if (temp < minValP2)
				minValP2 = temp;

			temp = (poseList[j].X_dot.block<3,1>(0,0)-poseList[j].Xd_dot.block<3,1>(0,0)).maxCoeff();
			if (temp > maxValP2)
				maxValP2 = temp;
		}
	}
	else if (errrpyPlot)
	{
		for (int j = 0; j < poseList.size(); j ++)
		{
			double temp = 0.0;

			temp = (poseList[j].X.block<3,1>(3,0)-poseList[j].Xd.block<3,1>(3,0)).minCoeff();
			if (temp < minValP1)
				minValP1 = temp;

			temp = (poseList[j].X.block<3,1>(3,0)-poseList[j].Xd.block<3,1>(3,0)).maxCoeff();
			if (temp > maxValP1)
				maxValP1 = temp;

			temp = (poseList[j].X_dot.block<3,1>(3,0)-poseList[j].Xd_dot.block<3,1>(3,0)).minCoeff();
			if (temp < minValP2)
				minValP2 = temp;

			temp = (poseList[j].X_dot.block<3,1>(3,0)-poseList[j].Xd_dot.block<3,1>(3,0)).maxCoeff();
			if (temp > maxValP2)
				maxValP2 = temp;
		}
	}
	else if (compare1Plot)
	{
		for (int j = 0; j < poseList.size(); j ++)
		{
			double temp = 0.0;

			temp = (Vector2d(poseList[j].X(0), poseList[j].Xd(0))).minCoeff();
			if (temp < minValP1)
				minValP1 = temp;

			temp = (Vector2d(poseList[j].X(0), poseList[j].Xd(0))).maxCoeff();
			if (temp > maxValP1)
				maxValP1 = temp;

			temp = (Vector2d(poseList[j].X(1), poseList[j].Xd(1))).minCoeff();
			if (temp < minValP2)
				minValP2 = temp;

			temp = (Vector2d(poseList[j].X(1), poseList[j].Xd(1))).maxCoeff();
			if (temp > maxValP2)
				maxValP2 = temp;
		}
	}
	else if (compare2Plot)
	{
		for (int j = 0; j < poseList.size(); j ++)
		{
			double temp = 0.0;

			temp = (Vector2d(poseList[j].X(2), poseList[j].Xd(2))).minCoeff();
			if (temp < minValP1)
				minValP1 = temp;

			temp = (Vector2d(poseList[j].X(2), poseList[j].Xd(2))).maxCoeff();
			if (temp > maxValP1)
				maxValP1 = temp;

			temp = (Vector2d(poseList[j].X(5), poseList[j].Xd(5))).minCoeff();
			if (temp < minValP2)
				minValP2 = temp;

			temp = (Vector2d(poseList[j].X(5), poseList[j].Xd(5))).maxCoeff();
			if (temp > maxValP2)
				maxValP2 = temp;
		}
	}

	if (minValP1 > -1)
		minValP1 = -1;
	else
		minValP1 *= 1.05;

	if (minValP2 > -1)
		minValP2 = -1;
	else
		minValP2 *= 1.05;

	if (maxValP1 < 1)
		maxValP1 = 1;
	else
		maxValP1 *= 1.05;

	if (maxValP2 < 1)
		maxValP2 = 1;
	else
		maxValP2 *= 1.05;

    if (posPlot)
    {
    	ui->qwtPlot1->setAxisScale(QwtPlot::yLeft, minValP1, maxValP1);
    	ui->qwtPlot2->setAxisScale(QwtPlot::yLeft, minValP2, maxValP2);
    }
    else if (rpyPlot)
    {
    	ui->qwtPlot1->setAxisScale(QwtPlot::yLeft, (180/M_PI)*minValP1, (180/M_PI)*maxValP1);
    	ui->qwtPlot2->setAxisScale(QwtPlot::yLeft, (180/M_PI)*minValP2, (180/M_PI)*maxValP2);
    }
    else if (errposPlot)
	{
		ui->qwtPlot1->setAxisScale(QwtPlot::yLeft, minValP1, maxValP1);
		ui->qwtPlot2->setAxisScale(QwtPlot::yLeft, minValP2, maxValP2);
	}
    else if (errrpyPlot)
	{
		ui->qwtPlot1->setAxisScale(QwtPlot::yLeft, (180/M_PI)*minValP1, (180/M_PI)*maxValP1);
		ui->qwtPlot2->setAxisScale(QwtPlot::yLeft, (180/M_PI)*minValP2, (180/M_PI)*maxValP2);
	}
    else if (compare1Plot)
	{
		ui->qwtPlot1->setAxisScale(QwtPlot::yLeft, minValP1, maxValP1);
		ui->qwtPlot2->setAxisScale(QwtPlot::yLeft, minValP2, maxValP2);
	}
    else if (compare2Plot)
	{
		ui->qwtPlot1->setAxisScale(QwtPlot::yLeft, minValP1, maxValP1);
		ui->qwtPlot2->setAxisScale(QwtPlot::yLeft, (180/M_PI)*minValP2, (180/M_PI)*maxValP2);
	}

	ui->qwtPlot1->replot();
	ui->qwtPlot2->replot();
}

void MainWindow::DDSReadCallback(const TrajectoryMessage &msg)
{
	if (testToggle)
		return;

	trajectorymsg = msg;
	emit trajectoryReceived();
}

void MainWindow::addPoint(const LocalWaypointDriverInfo& p)
{
    poseList.append(p);

    if (poseList.size() > numOfPoints)
        poseList.remove(0);
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
    plot->setAxisTitle(QwtPlot::xBottom, "Points Gathered");
    plot->setAxisScale(QwtPlot::xBottom, 0, numOfPoints);
    plot->setAxisScale(QwtPlot::yLeft, -1.0, 1.0);

    // Setup Legend
    QwtLegend *plotLegend = new QwtLegend();
    //plotLegend->setFrameShape(QFrame::Box);
    plotLegend->setFrameShadow(QFrame::Plain);
    plot->insertLegend(plotLegend, QwtPlot::RightLegend);

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
    d_origin->setValue(0 + numOfPoints / 2.0, 0.0);
    d_origin->setLinePen(QPen(Qt::gray, 0.0, Qt::DashLine));
    d_origin->attach(plot);
}

// Function to setup curves with default values to use with plots.
void MainWindow::setupCurve(QwtPlotCurve *curve, QPen pen)
{
    // Create and setup Curves
    curve->setStyle(QwtPlotCurve::Lines);
    curve->setPen(pen);
    curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    curve->setPaintAttribute(QwtPlotCurve::ClipPolygons, false);

    curve->data();
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

// Setup DESIRED RPY and AngularVelocity plots
void MainWindow::on_actionRPY_Data_triggered()
{
	ui->statusBar->showMessage("Plotting Desired PY and Desired Angular Velocity");

	posPlot = false;
	rpyPlot = true;
	errposPlot = false;
	errrpyPlot = false;
	compare1Plot = false;
	compare2Plot = false;

	ui->qwtPlot1->setTitle("Desired Pitch and Yaw");
	ui->qwtPlot2->setTitle("Desired Angular Velocity");

	// Setup Plot 1 for RPY
	DataSeries *buffer1 = (DataSeries *)curve1_Plot1->data();
	buffer1->setFunction(PlotDesiredRPY);
	buffer1->setComponent('p');
	buffer1->fill(20.0, numOfPoints);

	DataSeries *buffer2 = (DataSeries *)curve2_Plot1->data();
	buffer2->setFunction(PlotDesiredRPY);
	buffer2->setComponent('y');
	buffer2->fill(20.0, numOfPoints);

	curve3_Plot1->setVisible(false);

	// Setup Plot 2 for Desired Angular Velocity
	DataSeries *buffer4 = (DataSeries *)curve1_Plot2->data();
	buffer4->setFunction(PlotDesiredAngularVelocity);
	buffer4->setComponent('p');
	buffer4->fill(20.0, numOfPoints);

	DataSeries *buffer5 = (DataSeries *)curve2_Plot2->data();
	buffer5->setFunction(PlotDesiredAngularVelocity);
	buffer5->setComponent('y');
	buffer5->fill(20.0, numOfPoints);

	curve3_Plot2->setVisible(false);

    ui->qwtPlot1->replot();
    ui->qwtPlot2->replot();
}

// Setup Desired Position and Velocity plots
void MainWindow::on_actionPOS_triggered()
{
	ui->statusBar->showMessage("Plotting Desired Position and Desired Velocity");

	posPlot = true;
	rpyPlot = false;
	errposPlot = false;
	errrpyPlot = false;
	compare1Plot = false;
	compare2Plot = false;

	ui->qwtPlot1->setTitle("Desired Position");
	ui->qwtPlot2->setTitle("Desired Velocity");

	curve2_Plot1->setVisible(true);
	curve3_Plot1->setVisible(true);
	curve2_Plot2->setVisible(true);
	curve3_Plot2->setVisible(true);

	// Setup Plot 1 for RPY
	DataSeries *buffer1 = (DataSeries *)curve1_Plot1->data();
	buffer1->setFunction(PlotDesiredPosition);
	buffer1->setComponent('x');
	buffer1->fill(20.0, numOfPoints);

	DataSeries *buffer2 = (DataSeries *)curve2_Plot1->data();
	buffer2->setFunction(PlotDesiredPosition);
	buffer2->setComponent('y');
	buffer2->fill(20.0, numOfPoints);

	DataSeries *buffer3 = (DataSeries *)curve3_Plot1->data();
	buffer3->setFunction(PlotDesiredPosition);
	buffer3->setComponent('z');
	buffer3->fill(20.0, numOfPoints);

	// Setup Plot 2 for Desired Angular Velocity
	DataSeries *buffer4 = (DataSeries *)curve1_Plot2->data();
	buffer4->setFunction(PlotDesiredVelocity);
	buffer4->setComponent('x');
	buffer4->fill(20.0, numOfPoints);

	DataSeries *buffer5 = (DataSeries *)curve2_Plot2->data();
	buffer5->setFunction(PlotDesiredVelocity);
	buffer5->setComponent('y');
	buffer5->fill(20.0, numOfPoints);

	DataSeries *buffer6 = (DataSeries *)curve3_Plot2->data();
	buffer6->setFunction(PlotDesiredVelocity);
	buffer6->setComponent('z');
	buffer6->fill(20.0, numOfPoints);

    ui->qwtPlot1->replot();
    ui->qwtPlot2->replot();
}


// Plot Position and Velocity Error
void MainWindow::on_actionPos_Vel_Error_triggered()
{
	ui->statusBar->showMessage("Plotting Error in Position and Velocity");

	posPlot = false;
	rpyPlot = false;
	errposPlot = true;
	errrpyPlot = false;
	compare1Plot = false;
	compare2Plot = false;

	ui->qwtPlot1->setTitle("Error in Position");
	ui->qwtPlot2->setTitle("Error in Velocity");

	curve2_Plot1->setVisible(true);
	curve3_Plot1->setVisible(true);
	curve2_Plot2->setVisible(true);
	curve3_Plot2->setVisible(true);

	// Setup Plot 1 for RPY
	DataSeries *buffer1 = (DataSeries *)curve1_Plot1->data();
	buffer1->setFunction(PlotPositionError);
	buffer1->setComponent('x');
	buffer1->fill(20.0, numOfPoints);

	DataSeries *buffer2 = (DataSeries *)curve2_Plot1->data();
	buffer2->setFunction(PlotPositionError);
	buffer2->setComponent('y');
	buffer2->fill(20.0, numOfPoints);

	DataSeries *buffer3 = (DataSeries *)curve3_Plot1->data();
	buffer3->setFunction(PlotPositionError);
	buffer3->setComponent('z');
	buffer3->fill(20.0, numOfPoints);

	// Setup Plot 2 for Desired Angular Velocity
	DataSeries *buffer4 = (DataSeries *)curve1_Plot2->data();
	buffer4->setFunction(PlotVelocityError);
	buffer4->setComponent('x');
	buffer4->fill(20.0, numOfPoints);

	DataSeries *buffer5 = (DataSeries *)curve2_Plot2->data();
	buffer5->setFunction(PlotVelocityError);
	buffer5->setComponent('y');
	buffer5->fill(20.0, numOfPoints);

	DataSeries *buffer6 = (DataSeries *)curve3_Plot2->data();
	buffer6->setFunction(PlotVelocityError);
	buffer6->setComponent('z');
	buffer6->fill(20.0, numOfPoints);

    ui->qwtPlot1->replot();
    ui->qwtPlot2->replot();
}

// Plot RPY and Angular Velocity Error
void MainWindow::on_actionActionErrorRPY_triggered()
{
	ui->statusBar->showMessage("Plotting Error in PY and Angular Velocity");

	posPlot = false;
	rpyPlot = false;
	errposPlot = false;
	errrpyPlot = true;
	compare1Plot = false;
	compare2Plot = false;

	ui->qwtPlot1->setTitle("Error in Pitch and Yaw");
	ui->qwtPlot2->setTitle("Error in Angular Velocity");

	curve2_Plot1->setVisible(true);
	curve3_Plot1->setVisible(false);
	curve2_Plot2->setVisible(true);
	curve3_Plot2->setVisible(false);

	// Setup Plot 1 for RPY
	DataSeries *buffer1 = (DataSeries *)curve1_Plot1->data();
	buffer1->setFunction(PlotRPYError);
	buffer1->setComponent('p');
	buffer1->fill(20.0, numOfPoints);

	DataSeries *buffer2 = (DataSeries *)curve2_Plot1->data();
	buffer2->setFunction(PlotRPYError);
	buffer2->setComponent('y');
	buffer2->fill(20.0, numOfPoints);

	// Setup Plot 2 for Desired Angular Velocity
	DataSeries *buffer4 = (DataSeries *)curve1_Plot2->data();
	buffer4->setFunction(PlotAngularVelocityError);
	buffer4->setComponent('p');
	buffer4->fill(20.0, numOfPoints);

	DataSeries *buffer5 = (DataSeries *)curve2_Plot2->data();
	buffer5->setFunction(PlotAngularVelocityError);
	buffer5->setComponent('y');
	buffer5->fill(20.0, numOfPoints);

    ui->qwtPlot1->replot();
    ui->qwtPlot2->replot();
}

// ADDED FOR SIMULATION
void MainWindow::timerEvent(QTimerEvent *)
{
	TrajectoryInfo traj = trajectoryGenerator.Update(getTimestamp());

	addPoint(LocalWaypointDriverInfo(0,getTimestamp(), Matrix<double, 6, 1>::Zero(), Matrix<double, 6, 1>::Zero(), Matrix<double, 6, 1>::Zero(), traj.getTrajectory(), traj.getTrajectory_dot()));

    DataSeries *buffer1 = (DataSeries *)curve1_Plot1->data();
    buffer1->update(poseList.size());//poseList.size());

    DataSeries *buffer2 = (DataSeries *)curve2_Plot1->data();
    buffer2->update(poseList.size());//poseList.size());

    DataSeries *buffer3 = (DataSeries *)curve3_Plot1->data();
    buffer3->update(poseList.size());//poseList.size());

    DataSeries *buffer4 = (DataSeries *)curve1_Plot2->data();
    buffer4->update(poseList.size());//poseList.size());

    DataSeries *buffer5 = (DataSeries *)curve2_Plot2->data();
    buffer5->update(poseList.size());//poseList.size());

    DataSeries *buffer6 = (DataSeries *)curve3_Plot2->data();
    buffer6->update(poseList.size());//poseList.size());

    double minValP1 = 0.0, maxValP1 = 0.0, minValP2 = 0.0, maxValP2 = 0.0;

    if (posPlot)
	{
		for (int j = 0; j < poseList.size(); j ++)
		{
			double temp = 0.0;

			temp = (poseList[j].Xd.block<3,1>(0,0)).minCoeff();
			if (temp < minValP1)
				minValP1 = temp;

			temp = (poseList[j].Xd.block<3,1>(0,0)).maxCoeff();
			if (temp > maxValP1)
				maxValP1 = temp;

			temp = (poseList[j].Xd_dot.block<3,1>(0,0)).minCoeff();
			if (temp < minValP2)
				minValP2 = temp;

			temp = (poseList[j].Xd_dot.block<3,1>(0,0)).maxCoeff();
			if (temp > maxValP2)
				maxValP2 = temp;
		}
	}
	else if (rpyPlot)
	{
		for (int j = 0; j < poseList.size(); j ++)
		{
			double temp = 0.0;

			temp = (poseList[j].Xd.block<3,1>(3,0)).minCoeff();
			if (temp < minValP1)
				minValP1 = temp;

			temp = (poseList[j].Xd.block<3,1>(3,0)).maxCoeff();
			if (temp > maxValP1)
				maxValP1 = temp;

			temp = (poseList[j].Xd_dot.block<3,1>(3,0)).minCoeff();
			if (temp < minValP2)
				minValP2 = temp;

			temp = (poseList[j].Xd_dot.block<3,1>(3,0)).maxCoeff();
			if (temp > maxValP2)
				maxValP2 = temp;
		}
	}
	else if (errposPlot)
	{
		for (int j = 0; j < poseList.size(); j ++)
		{
			double temp = 0.0;

			temp = (poseList[j].X.block<3,1>(0,0)-poseList[j].Xd.block<3,1>(0,0)).minCoeff();
			if (temp < minValP1)
				minValP1 = temp;

			temp = (poseList[j].X.block<3,1>(0,0)-poseList[j].Xd.block<3,1>(0,0)).maxCoeff();
			if (temp > maxValP1)
				maxValP1 = temp;

			temp = (poseList[j].X_dot.block<3,1>(0,0)-poseList[j].Xd_dot.block<3,1>(0,0)).minCoeff();
			if (temp < minValP2)
				minValP2 = temp;

			temp = (poseList[j].X_dot.block<3,1>(0,0)-poseList[j].Xd_dot.block<3,1>(0,0)).maxCoeff();
			if (temp > maxValP2)
				maxValP2 = temp;
		}
	}
	else if (errrpyPlot)
	{
		for (int j = 0; j < poseList.size(); j ++)
		{
			double temp = 0.0;

			temp = (poseList[j].X.block<3,1>(3,0)-poseList[j].Xd.block<3,1>(3,0)).minCoeff();
			if (temp < minValP1)
				minValP1 = temp;

			temp = (poseList[j].X.block<3,1>(3,0)-poseList[j].Xd.block<3,1>(3,0)).maxCoeff();
			if (temp > maxValP1)
				maxValP1 = temp;

			temp = (poseList[j].X_dot.block<3,1>(3,0)-poseList[j].Xd_dot.block<3,1>(3,0)).minCoeff();
			if (temp < minValP2)
				minValP2 = temp;

			temp = (poseList[j].X_dot.block<3,1>(3,0)-poseList[j].Xd_dot.block<3,1>(3,0)).maxCoeff();
			if (temp > maxValP2)
				maxValP2 = temp;
		}
	}
	else if (compare1Plot)
	{
		for (int j = 0; j < poseList.size(); j ++)
		{
			double temp = 0.0;

			temp = (Vector2d(poseList[j].X(0), poseList[j].Xd(0))).minCoeff();
			if (temp < minValP1)
				minValP1 = temp;

			temp = (Vector2d(poseList[j].X(0), poseList[j].Xd(0))).maxCoeff();
			if (temp > maxValP1)
				maxValP1 = temp;

			temp = (Vector2d(poseList[j].X(1), poseList[j].Xd(1))).minCoeff();
			if (temp < minValP2)
				minValP2 = temp;

			temp = (Vector2d(poseList[j].X(1), poseList[j].Xd(1))).maxCoeff();
			if (temp > maxValP2)
				maxValP2 = temp;
		}
	}
	else if (compare2Plot)
	{
		for (int j = 0; j < poseList.size(); j ++)
		{
			double temp = 0.0;

			temp = (Vector2d(poseList[j].X(2), poseList[j].Xd(2))).minCoeff();
			if (temp < minValP1)
				minValP1 = temp;

			temp = (Vector2d(poseList[j].X(2), poseList[j].Xd(2))).maxCoeff();
			if (temp > maxValP1)
				maxValP1 = temp;

			temp = (Vector2d(poseList[j].X(5), poseList[j].Xd(5))).minCoeff();
			if (temp < minValP2)
				minValP2 = temp;

			temp = (Vector2d(poseList[j].X(5), poseList[j].Xd(5))).maxCoeff();
			if (temp > maxValP2)
				maxValP2 = temp;
		}
	}

    if (minValP1 > -1)
    	minValP1 = -1;
    else
    	minValP1 *= 1.05;

    if (minValP2 > -1)
		minValP2 = -1;
	else
		minValP2 *= 1.05;

    if (maxValP1 < 1)
    	maxValP1 = 1;
    else
    	maxValP1 *= 1.05;

    if (maxValP2 < 1)
    	maxValP2 = 1;
    else
    	maxValP2 *= 1.05;

    if (posPlot)
    {
    	ui->qwtPlot1->setAxisScale(QwtPlot::yLeft, minValP1, maxValP1);
    	ui->qwtPlot2->setAxisScale(QwtPlot::yLeft, minValP2, maxValP2);
    }
    else if (rpyPlot)
    {
    	ui->qwtPlot1->setAxisScale(QwtPlot::yLeft, (180/M_PI)*minValP1, (180/M_PI)*maxValP1);
    	ui->qwtPlot2->setAxisScale(QwtPlot::yLeft, (180/M_PI)*minValP2, (180/M_PI)*maxValP2);
    }
    else if (errposPlot)
	{
		ui->qwtPlot1->setAxisScale(QwtPlot::yLeft, minValP1, maxValP1);
		ui->qwtPlot2->setAxisScale(QwtPlot::yLeft, minValP2, maxValP2);
	}
    else if (errrpyPlot)
	{
		ui->qwtPlot1->setAxisScale(QwtPlot::yLeft, (180/M_PI)*minValP1, (180/M_PI)*maxValP1);
		ui->qwtPlot2->setAxisScale(QwtPlot::yLeft, (180/M_PI)*minValP2, (180/M_PI)*maxValP2);
	}
    else if (compare1Plot)
	{
		ui->qwtPlot1->setAxisScale(QwtPlot::yLeft, minValP1, maxValP1);
		ui->qwtPlot2->setAxisScale(QwtPlot::yLeft, minValP2, maxValP2);
	}
    else if (compare2Plot)
	{
		ui->qwtPlot1->setAxisScale(QwtPlot::yLeft, minValP1, maxValP1);
		ui->qwtPlot2->setAxisScale(QwtPlot::yLeft, (180/M_PI)*minValP2, (180/M_PI)*maxValP2);
	}

    ui->qwtPlot1->replot();
    ui->qwtPlot2->replot();
}

boost::int64_t MainWindow::getTimestamp(void)
{
	timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);

	return ((long long int)t.tv_sec * NSEC_PER_SEC) + t.tv_nsec;
}

void MainWindow::on_btnSubmitWaypt_clicked()
{
	ui->statusBar->showMessage("Waypoint Submitted");

	if (testToggle)
	{
		trajectoryGenerator.InitTimers(getTimestamp());

		Waypoint wp;

		wp.setX(ui->lineEditWayptX->text().toDouble());
		wp.setY(ui->lineEditWayptY->text().toDouble());
		wp.setZ(ui->lineEditWayptZ->text().toDouble());
		wp.setPitch(M_PI/ 180.0 * ui->lineEditWayptPitch->text().toDouble());
		wp.setYaw(M_PI / 180.0 * ui->lineEditWayptYaw->text().toDouble());

		trajectoryGenerator.SetWaypoint(wp, true);
	}
	else
	{
		SetWaypointMessage *msg = SetWaypointMessageTypeSupport::create_data();
		msg->isRelative = true;
		msg->position_ned[0] = ui->lineEditWayptX->text().toDouble();
		msg->position_ned[1] = ui->lineEditWayptY->text().toDouble();
		msg->position_ned[2] = ui->lineEditWayptZ->text().toDouble();
		msg->rpy[0] = 0;
		msg->rpy[1] = M_PI/ 180.0 * ui->lineEditWayptPitch->text().toDouble();
		msg->rpy[2] = M_PI/ 180.0 * ui->lineEditWayptYaw->text().toDouble();
		waypointddssender.Send(*msg);

		SetWaypointMessageTypeSupport::delete_data(msg);
	}
}

void MainWindow::on_btnSubmitStart_clicked()
{
	if (updateToggle)
	{
		updateToggle = false;

		ui->btnSubmitStart->setText("Submit Start");

		ui->statusBar->showMessage("Plotting Stopped");

		killTimer(d_timerId);
	}
	else
	{
		updateToggle = true;

		ui->btnSubmitStart->setText("Submit Stop");

		if (posPlot)
			ui->statusBar->showMessage("Plotting Position and Desired Velocity");
		else if (rpyPlot)
			ui->statusBar->showMessage("Plotting RPY and Desired Angular Velocity");

		d_timerId = startTimer(updateInterval);
	}
}

void MainWindow::on_btnCallUpdate_clicked()
{
	ui->statusBar->showMessage("Update Called");

	//addPoint(LocalWaypointDriverInfo(0,getTimestamp(), Matrix<double, 6, 1>::Zero(), x, x_dot, xd, xd_dot));

	DataSeries *buffer1 = (DataSeries *)curve1_Plot1->data();
	buffer1->update(poseList.size());//poseList.size());

	DataSeries *buffer2 = (DataSeries *)curve2_Plot1->data();
	buffer2->update(poseList.size());//poseList.size());

	DataSeries *buffer3 = (DataSeries *)curve3_Plot1->data();
	buffer3->update(poseList.size());//poseList.size());

	DataSeries *buffer4 = (DataSeries *)curve1_Plot2->data();
	buffer4->update(poseList.size());//poseList.size());

	DataSeries *buffer5 = (DataSeries *)curve2_Plot2->data();
	buffer5->update(poseList.size());//poseList.size());

	DataSeries *buffer6 = (DataSeries *)curve3_Plot2->data();
	buffer6->update(poseList.size());//poseList.size());

	ui->qwtPlot1->replot();
	ui->qwtPlot2->replot();
}

void MainWindow::on_btnToggleActual_clicked()
{
	if (actualToggle)
		actualToggle = false;
	else
		actualToggle = true;
}

void MainWindow::on_tabWidget_currentChanged(int index)
{

}

void MainWindow::on_btnSubmitGains_clicked()
{
	ControllerGainsMessage *msg = ControllerGainsMessageTypeSupport::create_data();

	msg->k[0] = ui->spinBoxkx->value();
	msg->k[1] = ui->spinBoxky->value();
	msg->k[2] = ui->spinBoxkz->value();
	msg->k[3] = ui->spinBoxkroll->value();
	msg->k[4] = ui->spinBoxkpitch->value();
	msg->k[5] = ui->spinBoxkyaw->value();

	msg->ks[0] = ui->spinBoxksx->value();
	msg->ks[1] = ui->spinBoxksy->value();
	msg->ks[2] = ui->spinBoxksz->value();
	msg->ks[3] = ui->spinBoxksroll->value();
	msg->ks[4] = ui->spinBoxkspitch->value();
	msg->ks[5] = ui->spinBoxksyaw->value();

	msg->alpha[0] = ui->spinBoxax->value();
	msg->alpha[1] = ui->spinBoxay->value();
	msg->alpha[2] = ui->spinBoxaz->value();
	msg->alpha[3] = ui->spinBoxaroll->value();
	msg->alpha[4] = ui->spinBoxapitch->value();
	msg->alpha[5] = ui->spinBoxayaw->value();

	msg->beta[0] = ui->spinBoxbx->value();
	msg->beta[1] = ui->spinBoxby->value();
	msg->beta[2] = ui->spinBoxbz->value();
	msg->beta[3] = ui->spinBoxbroll->value();
	msg->beta[4] = ui->spinBoxbpitch->value();
	msg->beta[5] = ui->spinBoxbyaw->value();

	cout << "Test Send Gains" << endl;
	gainsddssender.Send(*msg);

	ControllerGainsMessageTypeSupport::delete_data(msg);
}

// Plot Desired vs Actual x and y values
void MainWindow::on_actionDesired_vs_Actual_x_and_y_triggered()
{
	ui->statusBar->showMessage("Plotting Desired vs Actual X and Y Values");

	posPlot = false;
	rpyPlot = false;
	errposPlot = false;
	errrpyPlot = false;
	compare1Plot = true;
	compare2Plot = false;

	ui->qwtPlot1->setTitle("Desired vs Actual X");
	ui->qwtPlot2->setTitle("Desired vs Actual Y");

	// Setup Plot 1 for RPY
	DataSeries *buffer1 = (DataSeries *)curve1_Plot1->data();
	buffer1->setFunction(PlotDesiredPosition);
	buffer1->setComponent('x');
	buffer1->fill(20.0, numOfPoints);

	DataSeries *buffer2 = (DataSeries *)curve2_Plot1->data();
	buffer2->setFunction(PlotActualPosition);
	buffer2->setComponent('x');
	buffer2->fill(20.0, numOfPoints);

	curve3_Plot1->setVisible(false);

	// Setup Plot 2 for Desired Angular Velocity
	DataSeries *buffer4 = (DataSeries *)curve1_Plot2->data();
	buffer4->setFunction(PlotDesiredPosition);
	buffer4->setComponent('y');
	buffer4->fill(20.0, numOfPoints);

	DataSeries *buffer5 = (DataSeries *)curve2_Plot2->data();
	buffer5->setFunction(PlotActualPosition);
	buffer5->setComponent('y');
	buffer5->fill(20.0, numOfPoints);

	curve3_Plot2->setVisible(false);

    ui->qwtPlot1->replot();
    ui->qwtPlot2->replot();
}

// Plot Desired vs Actual Z and Yaw Values
void MainWindow::on_actionDesired_vs_Actual_z_and_yaw_triggered()
{
	ui->statusBar->showMessage("Plotting Desired vs Actual Z and Yaw Values");

	posPlot = false;
	rpyPlot = false;
	errposPlot = false;
	errrpyPlot = false;
	compare1Plot = false;
	compare2Plot = true;

	ui->qwtPlot1->setTitle("Desired vs Actual Z");
	ui->qwtPlot2->setTitle("Desired vs Actual Yaw");

	// Setup Plot 1 for RPY
	DataSeries *buffer1 = (DataSeries *)curve1_Plot1->data();
	buffer1->setFunction(PlotDesiredPosition);
	buffer1->setComponent('z');
	buffer1->fill(20.0, numOfPoints);

	DataSeries *buffer2 = (DataSeries *)curve2_Plot1->data();
	buffer2->setFunction(PlotActualPosition);
	buffer2->setComponent('z');
	buffer2->fill(20.0, numOfPoints);

	curve3_Plot1->setVisible(false);

	// Setup Plot 2 for Desired Angular Velocity
	DataSeries *buffer4 = (DataSeries *)curve1_Plot2->data();
	buffer4->setFunction(PlotDesiredRPY);
	buffer4->setComponent('y');
	buffer4->fill(20.0, numOfPoints);

	DataSeries *buffer5 = (DataSeries *)curve2_Plot2->data();
	buffer5->setFunction(PlotActualRPY);
	buffer5->setComponent('y');
	buffer5->fill(20.0, numOfPoints);

	curve3_Plot2->setVisible(false);

    ui->qwtPlot1->replot();
    ui->qwtPlot2->replot();
}

void MainWindow::on_btnTestToggle_clicked()
{
	if (testToggle)
	{
		testToggle = false;

		ui->btnSubmitStart->setVisible(false);
		ui->btnCallUpdate->setVisible(false);
	}
	else
	{
		testToggle = true;

		ui->btnSubmitStart->setVisible(true);
		ui->btnCallUpdate->setVisible(true);

	    Waypoint wp;
	    trajectoryGenerator.SetWaypoint(wp, true);
	    trajectoryGenerator.InitTimers(getTimestamp());
	}
}
