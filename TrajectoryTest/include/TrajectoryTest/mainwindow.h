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
#include "SubMain/Workers/WaypointController/LocalWaypointDriverWorker.h"
#include <Eigen/Dense>
#include <time.h>
#include <cmath>

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

		explicit MainWindow(QWidget *parent = 0);
		~MainWindow();

		void start();

		void setupPlot(QwtPlot *plot);
		void setupCurve(QwtPlotCurve *curve, QPen pen);

		//void addPoint(points pos);
		void addPoint(LocalWaypointDriverDynamicInfo p);

		TrajectoryGenerator *trajectoryGenerator;

	protected:
		virtual void timerEvent(QTimerEvent *e);

	private slots:
		void on_actionRPY_triggered();
		void on_actionPOS_triggered();
		void on_actionError_triggered();
	    void on_btnSubmitWaypt_clicked();
	    void on_btnSubmitStart_clicked();
	    void on_btnCallUpdate_clicked();

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
	};
}
#endif // MAINWINDOW_H
