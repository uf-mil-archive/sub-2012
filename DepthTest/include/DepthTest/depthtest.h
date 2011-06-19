#ifndef DEPTHTEST_MAINWINDOW_H
#define DEPTHTEST_MAINWINDOW_H

#include "DepthTest/DepthBoardController.h"
#include "DepthTest/LoggerController.h"
#include "DataObjects/DepthBoard/DepthBoardInfo.h"
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
			DepthBoardController depthcontroller;
			LoggerController logger;

			Ui::DepthTest ui;
	};
}

#endif

