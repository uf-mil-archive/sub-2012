#ifndef PDTEST_MAINWINDOW_H
#define PDTEST_MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"
#include "DDSListeners/DDSSender.h"
#include "DDSMessages/PDWrenchMessage.h"
#include "DDSMessages/PDWrenchMessageSupport.h"

namespace subjugator {
	class MainWindow : public QMainWindow {
		Q_OBJECT

		public:
			MainWindow();

		private slots:
			void onSendButtonClicked();
			void onStopButtonClicked();

		private:
			Ui::MainWindow ui;

			class DDSHelper {
				public:
					DDSHelper();
					~DDSHelper();

					DDSDomainParticipant *participant;
			};
			DDSHelper dds;

			DDSSender<PDWrenchMessage, PDWrenchMessageDataWriter, PDWrenchMessageTypeSupport> sender;
	};
}

#endif

