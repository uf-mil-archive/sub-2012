#ifndef DEPTHTEST_MAINWINDOW_H
#define DEPTHTEST_MAINWINDOW_H

#include "DepthTest/DepthController.h"
#include "DepthTest/LoggerController.h"
#include "DataObjects/Depth/DepthInfo.h"
#include <QMainWindow>
#include "ui_depthtest.h"

namespace subjugator {
	class MainWindow : public QMainWindow {
		Q_OBJECT

		public:
			MainWindow(int haladdr=4);

		private slots:
			void onStartLogButtonClicked();
			void onStopLogButtonClicked();
			void onBrowseButtonClicked();
			void onNewDepthInfo();

		private:
			DepthController depthcontroller;
			LoggerController logger;

			Ui::DepthTest ui;
	};
}

#endif

