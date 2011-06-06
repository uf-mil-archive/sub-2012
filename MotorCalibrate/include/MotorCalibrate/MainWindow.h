#ifndef MOTORCALIBRATE_MAINWINDOW_H
#define MOTORCALIBRATE_MAINWINDOW_H

#include "HeartBeatSender.h"
#include "HAL/SubHAL.h"
#include "HAL/format/DataObjectEndpoint.h"
#include <boost/scoped_ptr.hpp>
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

		private:
			SubHAL hal;
			boost::scoped_ptr<DataObjectEndpoint> endpoint;

			HeartBeatSender heartbeatsender;

			Ui::MainWindow ui;
	};
}

#endif

