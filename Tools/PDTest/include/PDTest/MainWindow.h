#ifndef PDTEST_MAINWINDOW_H
#define PDTEST_MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "ui_mainwindow.h"
#include "DDSListeners/DDSSender.h"
#include "DDSMessages/PDWrenchMessage.h"
#include "DDSMessages/PDWrenchMessageSupport.h"
#include "DDSMessages/PDActuatorMessage.h"
#include "DDSMessages/PDActuatorMessageSupport.h"
#include "DDSMessages/PDStatusMessage.h"
#include "DDSMessages/PDStatusMessageSupport.h"
#include "DDSCommanders/DDSReceiver.h"

namespace subjugator {
	class MainWindow : public QMainWindow {
		Q_OBJECT

		public:
			MainWindow();

		private slots:
			void onSendButtonClicked();
			void onStopButtonClicked();
			void onPDStatusUpdated();
			void onDeactivateTimer();

		signals:
			void pdStatusUpdated();

		private:
			Ui::MainWindow ui;
			QTimer deactivatetimer;

			class DDSHelper {
				public:
					DDSHelper();
					~DDSHelper();

					DDSDomainParticipant *participant;
			};
			DDSHelper dds;

			DDSSender<PDWrenchMessage, PDWrenchMessageDataWriter, PDWrenchMessageTypeSupport> sender;
			DDSSender<PDActuatorMessage, PDActuatorMessageDataWriter, PDActuatorMessageTypeSupport> actuatorsender;
			DDSReceiver<PDStatusMessage, PDStatusMessageDataReader, PDStatusMessageTypeSupport, PDStatusMessageSeq> receiver;

			PDStatusMessage pdstatus;

			void onPDStatusMessage(const PDStatusMessage &pdstatus);

			int genFlagsFromCheckboxes() const;
	};
}

#endif

