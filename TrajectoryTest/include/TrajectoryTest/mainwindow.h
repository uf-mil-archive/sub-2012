#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qwt_plot.h>
#include <QWidget>
#include <qwt_system_clock.h>
#include <QPen>
#include <QVector>
#include "ui_mainwindow.h"
#include "dataseries.h"
#include "SubMain/Workers/WaypointController/TrajectoryGenerator.h"
#include "DDSCommanders/TrajectoryDDSReceiver.h"
#include "DataObjects/Trajectory/TrajectoryInfo.h"
#include "DataObjects/LocalWaypointDriver/LocalWaypointDriverInfo.h"
#include "DDSMessages/SetWaypointMessage.h"
#include "DDSMessages/SetWaypointMessageSupport.h"
#include "DDSMessages/ControllerGainsMessage.h"
#include "DDSMessages/ControllerGainsMessageSupport.h"
#include "DDSListeners/DDSSender.h"
#include <ndds/ndds_cpp.h>
#include <Eigen/Dense>
#include <time.h>
#include <cmath>
#include <algorithm>

#define _USE_MATH_DEFINES
#define NSEC_PER_SEC 1000000000

class QwtPlotCurve;
class QwtPlotMarker;
class QwtPlotDirectPainter;

class Plot;

using namespace Eigen;

namespace Ui {
	class MainWindow;
}

namespace subjugator
{
	class MainWindow : public QMainWindow
	{
		Q_OBJECT

	public:
		typedef Matrix<double, 6, 1> Vector6d;

		explicit MainWindow(DDSDomainParticipant *participant, DDSDomainParticipant *partSender, QWidget *parent = 0);
		~MainWindow();

		void start();

		void setupPlot(QwtPlot *plot);
		void setupCurve(QwtPlotCurve *curve, QPen pen);

		//void addPoint(points pos);
		void addPoint(const LocalWaypointDriverInfo& p);

		TrajectoryGenerator trajectoryGenerator;
		void DDSReadCallback(const TrajectoryMessage &msg);

	protected:
		virtual void timerEvent(QTimerEvent *e);

	private slots:
		void on_actionRPY_Data_triggered();
		void on_actionPOS_triggered();
		void on_actionPos_Vel_Error_triggered();
		void on_actionActionErrorRPY_triggered();
	    void on_btnSubmitWaypt_clicked();
	    void on_btnSubmitStart_clicked();
	    void on_btnCallUpdate_clicked();
	    void onTrajectoryReceived();
	    void on_btnToggleActual_clicked();
	    void on_tabWidget_currentChanged(int index);
	    void on_btnSubmitGains_clicked();
	    void on_actionDesired_vs_Actual_x_and_y_triggered();
	    void on_actionDesired_vs_Actual_z_and_yaw_triggered();
	    void on_btnTestToggle_clicked();

	    signals:
	    void trajectoryReceived();

	private:
	    void initGradient(QwtPlot *plot);

		boost::int64_t getTimestamp(void);
		Ui::MainWindow *ui;

		QwtPlotMarker *d_origin;
		QwtPlotCurve *curve1_Plot1;
		QwtPlotCurve *curve2_Plot1;
		QwtPlotCurve *curve3_Plot1;
		QwtPlotCurve *curve1_Plot2;
		QwtPlotCurve *curve2_Plot2;
		QwtPlotCurve *curve3_Plot2;

		QwtPlotDirectPainter *d_directPainter;
		QwtSystemClock d_clock;
		QPen pen;

		int d_paintedPoints;
		QwtInterval d_interval;
		int d_timerId;
		double testval;
		int numOfPoints;
		int updateInterval;
		bool updateToggle;

		bool posPlot;
		bool rpyPlot;
		bool errposPlot;
		bool errrpyPlot;
		bool compare1Plot;
		bool compare2Plot;
		bool actualToggle;
		bool testToggle;

		TrajectoryDDSReceiver trajectoryreceiver;

		TrajectoryMessage trajectorymsg;

		DDSSender<SetWaypointMessage, SetWaypointMessageDataWriter, SetWaypointMessageTypeSupport> waypointddssender;
		DDSSender<ControllerGainsMessage, ControllerGainsMessageDataWriter, ControllerGainsMessageTypeSupport> gainsddssender;
	};
}
#endif // MAINWINDOW_H
