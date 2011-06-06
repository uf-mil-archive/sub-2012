#ifndef MOTORCALIBRATE_MAINWINDOW_H
#define MOTORCALIBRATE_MAINWINDOW_H

#include "MotorCalibrate/MotorDriverController.h"
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
			void onNewMotorInfo(const MotorDriverInfo &info);

		private:
			MotorDriverController motorcontroller;

			Ui::MainWindow ui;
	};
}

#endif

