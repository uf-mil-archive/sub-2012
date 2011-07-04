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

#include "DataObjects/Trajectory/TrajectoryInfo.h"

using namespace subjugator;
using namespace std;
using namespace Eigen;

QVector<TrajectoryInfo> poseList;
QVector<double> testPts;

static points PlotPosition(int index, char c)
{
	points pos;

	pos.x = index;

	if (c == 'x')
		pos.y = (poseList[index].getTrajectory())(0);
	else if (c == 'y')
		pos.y = (poseList[index].getTrajectory())(1);
	else if (c == 'z')
		pos.y = (poseList[index].getTrajectory())(2);
	else
		pos.y = 0;

	return pos;
}


static points PlotDesiredVelocity(int index, char c)
{
	points pos;

	pos.x = index;

	if (c == 'x')
		pos.y = (poseList[index].getTrajectory_dot())(0);
	else if (c == 'y')
		pos.y = (poseList[index].getTrajectory_dot())(1);
	else if (c == 'z')
		pos.y = (poseList[index].getTrajectory_dot())(2);
	else
		pos.y = 0;

	return pos;
}

static points PlotRPY(int index, char c)
{
	points pos;

	pos.x = index;
	if(c == 'p')
		pos.y = (poseList[index].getTrajectory())(4);
	else if(c == 'y')
		pos.y = (poseList[index].getTrajectory())(5);
	else
		pos.y = 0;

	return pos;
}

static points PlotDesiredAngularVelocity(int index, char c)
{
	points pos;

	pos.x = index;
	if(c == 'p')
		pos.y = (poseList[index].getTrajectory_dot())(4);
	else if(c == 'y')
		pos.y = (poseList[index].getTrajectory_dot())(5);
	else
		pos.y = 0;

	return pos;
}

static points pointTest(int index, char c)
{
	points pos;

    pos.x = index;

    if (c == 'r')
    	pos.y = (double)index/500.0;
    else if (c == 's')
    	pos.y = sin(3.14/index);
    else if (c == 't')
    	pos.y = testPts.data()[index];

    return pos;
}

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
	updateToggle(true),
	posPlot(true),
	rpyPlot(false),
	trajectoryreceiver(participant, "Trajectory", bind(&MainWindow::DDSReadCallback, this, _1)),
	ddssender(partSender, "LocalWaypointDriver")
{
    testval = 0;
    testPts.resize(numOfPoints);
    testPts.fill(0);

    poseList.resize(numOfPoints);

    Waypoint wp;
    trajectoryGenerator.SetWaypoint(wp, true);
    trajectoryGenerator.InitTimers(getTimestamp());

    ui->setupUi(this);
    ui->statusBar->showMessage("Plotting Stopped");

    connect(this, SIGNAL(trajectoryReceived()), this, SLOT(onTrajectoryReceived()));

    curve1_Plot1  = new QwtPlotCurve("Pos X");
    curve2_Plot1  = new QwtPlotCurve("Pos Y");
    curve3_Plot1  = new QwtPlotCurve("Pos Z");
    curve1_Plot2  = new QwtPlotCurve("Vel X");
    curve2_Plot2  = new QwtPlotCurve("Vel Y");
    curve3_Plot2  = new QwtPlotCurve("Vel Z");

    setupPlot(ui->qwtPlot1);
    setupPlot(ui->qwtPlot2);
    setupCurve(curve1_Plot1, QPen(Qt::cyan));
    setupCurve(curve2_Plot1, QPen(Qt::green));
    setupCurve(curve3_Plot1, QPen(Qt::yellow));
    setupCurve(curve1_Plot2, QPen(Qt::cyan));
	setupCurve(curve2_Plot2, QPen(Qt::green));
	setupCurve(curve3_Plot2, QPen(Qt::yellow));

    curve1_Plot1->setData(new DataSeries(20.0, numOfPoints));
    curve2_Plot1->setData(new DataSeries(20.0, numOfPoints));
    curve3_Plot1->setData(new DataSeries(20.0, numOfPoints));
    curve1_Plot2->setData(new DataSeries(20.0, numOfPoints));
	curve2_Plot2->setData(new DataSeries(20.0, numOfPoints));
	curve3_Plot2->setData(new DataSeries(20.0, numOfPoints));

	// Set Three curves per plot.
	DataSeries *buffer1 = (DataSeries *)curve1_Plot1->data();
    buffer1->setFunction(PlotPosition);
    buffer1->setComponent('x');
    buffer1->fill(20.0, numOfPoints);

	DataSeries *buffer2 = (DataSeries *)curve2_Plot1->data();
    buffer2->setFunction(PlotPosition);
    buffer2->setComponent('y');
    buffer2->fill(20.0, numOfPoints);

	DataSeries *buffer3 = (DataSeries *)curve3_Plot1->data();
    buffer3->setFunction(PlotPosition);
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
    ui->qwtPlot1->setTitle("Position");
    ui->qwtPlot2->setTitle("Desired Velocity");

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
	qDebug() << " I GOT HERE, YEAH!!!!";
}

void MainWindow::DDSReadCallback(const TrajectoryMessage &msg)
{
	trajectoryinfo = msg;
	emit trajectoryReceived();
}

void MainWindow::addPoint(const TrajectoryInfo& p)
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
    plotLegend->setFrameShape(QFrame::Box);
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

// Setup RPY and DesiredAngularVelocity plots
void MainWindow::on_actionRPY_triggered()
{
	ui->statusBar->showMessage("Plotting RPY and Desired Angular Velocity");

	posPlot = false;
	rpyPlot = true;

	ui->qwtPlot1->setTitle("RPY");
	ui->qwtPlot2->setTitle("Desired Angular Velocity");

	// Setup Plot 1 for RPY
	DataSeries *buffer1 = (DataSeries *)curve1_Plot1->data();
	buffer1->setFunction(PlotRPY);
	buffer1->setComponent('p');
	buffer1->fill(20.0, numOfPoints);

	DataSeries *buffer2 = (DataSeries *)curve2_Plot1->data();
	buffer2->setFunction(PlotRPY);
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

// Setup Position and DesiredAngularVelocity plots
void MainWindow::on_actionPOS_triggered()
{
	ui->statusBar->showMessage("Plotting Position and Desired Velocity");

	posPlot = true;
	rpyPlot = false;

	ui->qwtPlot1->setTitle("Position");
	ui->qwtPlot2->setTitle("Desired Velocity");

	curve2_Plot1->setVisible(true);
	curve3_Plot1->setVisible(true);
	curve2_Plot2->setVisible(true);
	curve3_Plot2->setVisible(true);

	// Setup Plot 1 for RPY
	DataSeries *buffer1 = (DataSeries *)curve1_Plot1->data();
	buffer1->setFunction(PlotPosition);
	buffer1->setComponent('x');
	buffer1->fill(20.0, numOfPoints);

	DataSeries *buffer2 = (DataSeries *)curve2_Plot1->data();
	buffer2->setFunction(PlotPosition);
	buffer2->setComponent('y');
	buffer2->fill(20.0, numOfPoints);

	DataSeries *buffer3 = (DataSeries *)curve3_Plot1->data();
	buffer3->setFunction(PlotPosition);
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

void MainWindow::on_actionError_triggered()
{
	ui->statusBar->showMessage("Plotting Error");

	curve2_Plot1->setVisible(false);
	curve3_Plot1->setVisible(false);
	curve3_Plot2->setVisible(false);

	// Setup Plot 1 for RPY
	DataSeries *buffer1 = (DataSeries *)curve1_Plot1->data();
	buffer1->setFunction(pointTest);
	buffer1->setComponent('x');
	buffer1->fill(20.0, numOfPoints);

	// Setup Plot 2 for Desired Angular Velocity
	DataSeries *buffer4 = (DataSeries *)curve1_Plot2->data();
	buffer4->setFunction(pointTest);
	buffer4->setComponent('t');
	buffer4->fill(20.0, numOfPoints);

	// Setup Plot 2 for Desired Angular Velocity
	DataSeries *buffer5 = (DataSeries *)curve2_Plot2->data();
	buffer5->setFunction(pointTest);
	buffer5->setComponent('s');
	buffer5->fill(20.0, numOfPoints);

    ui->qwtPlot1->replot();
    ui->qwtPlot2->replot();
}

// ADDED FOR SIMULATION
void MainWindow::timerEvent(QTimerEvent *)
{
	addPoint(trajectoryGenerator.Update(getTimestamp()));

	testval+=0.05;

	if (testval > 1)
		testval = 0;

    testPts.append(testval);

    if (testPts.size() > numOfPoints)
    	testPts.remove(0,1);

    DataSeries *buffer1 = (DataSeries *)curve1_Plot1->data();
    buffer1->update(500);//poseList.size());

    DataSeries *buffer2 = (DataSeries *)curve2_Plot1->data();
    buffer2->update(500);//poseList.size());

    DataSeries *buffer3 = (DataSeries *)curve3_Plot1->data();
    buffer3->update(500);//poseList.size());

    DataSeries *buffer4 = (DataSeries *)curve1_Plot2->data();
    buffer4->update(500);//poseList.size());

    DataSeries *buffer5 = (DataSeries *)curve2_Plot2->data();
    buffer5->update(500);//poseList.size());

    DataSeries *buffer6 = (DataSeries *)curve3_Plot2->data();
    buffer6->update(500);//poseList.size());

    double minValP1 = 0.0, maxValP1 = 0.0, minValP2 = 0.0, maxValP2 = 0.0;

    if (posPlot)
    {
		for (int j = 0; j < poseList.size(); j ++)
		{
			double temp = 0.0;

			temp = (poseList[j].getTrajectory().block<3,1>(0,0)).minCoeff();
			if (temp < minValP1)
				minValP1 = temp;

			temp = (poseList[j].getTrajectory().block<3,1>(0,0)).maxCoeff();
			if (temp > maxValP1)
				maxValP1 = temp;

			temp = (poseList[j].getTrajectory_dot().block<3,1>(0,0)).minCoeff();
			if (temp < minValP2)
				minValP2 = temp;

			temp = (poseList[j].getTrajectory_dot().block<3,1>(0,0)).maxCoeff();
			if (temp > maxValP2)
				maxValP2 = temp;
		}
    }
    else if (rpyPlot)
    {
		for (int j = 0; j < poseList.size(); j ++)
		{
			double temp = 0.0;

			temp = (poseList[j].getTrajectory().block<3,1>(3,0)).minCoeff();
			if (temp < minValP1)
				minValP1 = temp;

			temp = (poseList[j].getTrajectory().block<3,1>(3,0)).maxCoeff();
			if (temp > maxValP1)
				maxValP1 = temp;

			temp = (poseList[j].getTrajectory_dot().block<3,1>(3,0)).minCoeff();
			if (temp < minValP2)
				minValP2 = temp;

			temp = (poseList[j].getTrajectory_dot().block<3,1>(3,0)).maxCoeff();
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

    ui->qwtPlot1->setAxisScale(QwtPlot::yLeft, minValP1, maxValP1);
    ui->qwtPlot2->setAxisScale(QwtPlot::yLeft, minValP2, maxValP2);

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

    Waypoint wp;

    wp.setX(ui->lineEditWayptX->text().toDouble());
    wp.setY(ui->lineEditWayptY->text().toDouble());
    wp.setZ(ui->lineEditWayptZ->text().toDouble());
    wp.setPitch(M_PI/ 180.0 * ui->lineEditWayptPitch->text().toDouble());
    wp.setYaw(M_PI / 180.0 * ui->lineEditWayptYaw->text().toDouble());

    trajectoryGenerator.SetWaypoint(wp, true);

    LocalWaypointDriverMessage *msg = LocalWaypointDriverMessageTypeSupport::create_data();
    msg->position_ned[0] = ui->lineEditWayptX->text().toDouble();
    msg->position_ned[1] = ui->lineEditWayptY->text().toDouble();
    msg->position_ned[2] = ui->lineEditWayptZ->text().toDouble();
    msg->rpy[0] = 0;
    msg->rpy[1] = M_PI/ 180.0 * ui->lineEditWayptPitch->text().toDouble();
    msg->rpy[2] = M_PI/ 180.0 * ui->lineEditWayptYaw->text().toDouble();
	ddssender.Send(*msg);

	LocalWaypointDriverMessageTypeSupport::delete_data(msg);
}

void MainWindow::on_btnSubmitStart_clicked()
{
	if (updateToggle)
	{
		if (posPlot)
			ui->statusBar->showMessage("Plotting Position and Desired Velocity");
		else if (rpyPlot)
			ui->statusBar->showMessage("Plotting RPY and Desired Angular Velocity");
		updateToggle = false;
		d_timerId = startTimer(updateInterval);
	}
	else
	{
		ui->statusBar->showMessage("Plotting Stopped");
		updateToggle = true;
		killTimer(d_timerId);
	}
}

void MainWindow::on_btnCallUpdate_clicked()
{
	ui->statusBar->showMessage("Update Called");

	addPoint(trajectoryGenerator.Update(getTimestamp()));

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