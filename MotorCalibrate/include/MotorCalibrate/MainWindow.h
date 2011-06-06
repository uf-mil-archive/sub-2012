#ifndef MOTORCALIBRATE_MAINWINDOW_H
#define MOTORCALIBRATE_MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"

namespace subjugator {
	class MainWindow : public QMainWindow {
		Q_OBJECT

		public:
			MainWindow(QWidget *parent=NULL);

		private:
			Ui::MainWindow ui;
	};
}

#endif

