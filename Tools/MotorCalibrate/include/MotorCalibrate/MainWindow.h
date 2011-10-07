#ifndef MOTORCALIBRATE_MAINWINDOW_H
#define MOTORCALIBRATE_MAINWINDOW_H

#include "MotorCalibrate/MotorDriverController.h"
#include "MotorCalibrate/LoggerController.h"
#include "MotorCalibrate/IMUSensorLogger.h"
#include "DataObjects/MotorDriver/MotorDriverInfo.h"
#include <QMainWindow>
#include "ui_mainwindow.h"

namespace subjugator {
	class MainWindow : public QMainWindow {
		Q_OBJECT

		public:
			MainWindow(int haladdr=2);

		private slots:
			void onSetReferenceButtonClicked();
			void onStopReferenceButtonClicked();
			void onStartRampButtonClicked();
			void onStopRampButtonClicked();
			void onStartBangButtonClicked();
			void onStopBangButtonClicked();
			void onConnectButtonClicked();
			void onStartLogButtonClicked();
			void onStopLogButtonClicked();
			void onBrowseButtonClicked();
			void onStartMagLogButtonClicked();
			void onStopMagLogButtonClicked();
			void onMagBrowseButtonClicked();
			void onNewMotorInfo();
			void onNewRampReference(double reference);
			void onRampComplete();
			void onNewBangReference(double reference);
			void onNewForce(double force);
			void onNewIMU();

		private:
			MotorDriverController motorcontroller;
			LoggerController logger;
			IMUSensorLogger imulogger;

			Ui::MainWindow ui;
	};
}

#endif

