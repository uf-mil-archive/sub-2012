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
#include "DDSMessages/LocalWaypointDriverMessage.h"
#include "DDSMessages/LocalWaypointDriverMessageSupport.h"
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
		void addPoint(const TrajectoryInfo& p);

		TrajectoryGenerator trajectoryGenerator;
		void DDSReadCallback(const TrajectoryMessage &msg);

	protected:
		virtual void timerEvent(QTimerEvent *e);

	private slots:
		void on_actionRPY_triggered();
		void on_actionPOS_triggered();
		void on_actionError_triggered();
	    void on_btnSubmitWaypt_clicked();
	    void on_btnSubmitStart_clicked();
	    void on_btnCallUpdate_clicked();
	    void onTrajectoryReceived();

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

		TrajectoryDDSReceiver trajectoryreceiver;

		TrajectoryMessage trajectoryinfo;

		DDSSender<LocalWaypointDriverMessage, LocalWaypointDriverMessageDataWriter, LocalWaypointDriverMessageTypeSupport> ddssender;
	};
}
#endif // MAINWINDOW_H