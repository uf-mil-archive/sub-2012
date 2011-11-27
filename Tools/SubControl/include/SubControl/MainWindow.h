#ifndef PDTEST_MAINWINDOW_H
#define PDTEST_MAINWINDOW_H

#include "WorkerManager/Messages/WorkerManagerStatusMessageSupport.h"
#include "WorkerManager/Messages/WorkerManagerCommandMessageSupport.h"
#include "LibSub/Messages/WorkerStateMessageSupport.h"
#include "LibSub/Messages/WorkerLogMessageSupport.h"
#include "LibSub/State/State.h"
#include "LibSub/DDS/Participant.h"
#include "LibSub/DDS/Receiver.h"
#include "LibSub/DDS/Sender.h"
#include "LibSub/DDS/Topic.h"
#include <QMainWindow>
#include <QTimer>
#include "ui_mainwindow.h"
#include <string>

DECLARE_MESSAGE_TRAITS(WorkerManagerStatusMessage);
DECLARE_MESSAGE_TRAITS(WorkerManagerCommandMessage);
DECLARE_MESSAGE_TRAITS(WorkerStateMessage);
DECLARE_MESSAGE_TRAITS(WorkerLogMessage);

namespace subjugator {
	enum CombinedState {
		STOPPED,
		STARTED,
		STOPPING,

		ACTIVE,
		STANDBY,
		ERROR,
	};

	class MainWindow : public QMainWindow {
		Q_OBJECT

		public:
			MainWindow();

		private slots:
			void update();
			void cellChanged(int row, int col);

		private:
			Ui::MainWindow ui;
			QTimer timer;
			bool updating;

			Participant part;
			Topic<WorkerManagerStatusMessage> statustopic;
			PollingReceiver<WorkerManagerStatusMessage> statusreceiver;
			Topic<WorkerManagerCommandMessage> commandtopic;
			Sender<WorkerManagerCommandMessage> commandsender;
			Topic<WorkerStateMessage> statetopic;
			PollingReceiver<WorkerStateMessage> statereceiver;
			Topic<WorkerLogMessage> logtopic;
			PollingReceiver<WorkerLogMessage> logreceiver;

			void updateWorkers();
			void updateLog();

			void addWorker(const std::string &name, CombinedState state, bool checkable, const std::string &msg);
	};

	std::ostream &operator<<(std::ostream &out, CombinedState state);
}

#endif

