#ifndef PDTEST_MAINWINDOW_H
#define PDTEST_MAINWINDOW_H

#include "SubControl/Stats.h"
#include "WorkerManager/Messages/WorkerManagerStatusMessageSupport.h"
#include "WorkerManager/Messages/WorkerManagerCommandMessageSupport.h"
#include "LibSub/Messages/WorkerStateMessageSupport.h"
#include "LibSub/Messages/WorkerLogMessageSupport.h"
#include "LibSub/Messages/WorkerKillMessageSupport.h"
#include "LibSub/State/State.h"
#include "DDS/Participant.h"
#include "DDS/Receiver.h"
#include "DDS/Sender.h"
#include "DDS/Topic.h"
#include <QMainWindow>
#include <QTimer>
#include "ui_mainwindow.h"
#include <string>
#include <boost/optional.hpp>

DECLARE_MESSAGE_TRAITS(WorkerManagerStatusMessage);
DECLARE_MESSAGE_TRAITS(WorkerManagerCommandMessage);
DECLARE_MESSAGE_TRAITS(WorkerStateMessage);
DECLARE_MESSAGE_TRAITS(WorkerLogMessage);
DECLARE_MESSAGE_TRAITS(WorkerKillMessage);

namespace subjugator {
	class MainWindow : public QMainWindow {
		Q_OBJECT

	public:
		MainWindow();

	private slots:
		void update();
		void cellChanged(int row, int col);
		void killClicked();
		void unkillClicked();

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
		Topic<WorkerKillMessage> killtopic;
		PollingReceiver<WorkerKillMessage> killreceiver;
		Sender<WorkerKillMessage> killsender;

		Stats stats;

		void updateWorkers();
		void updateKills();
		void updateLog();
		void updateStats();

		void updateWorker(int row, const std::string &name, const boost::optional<WorkerManagerProcessStatus> &status, const boost::optional<State> &state);
		void updateKill(int row, const WorkerKillMessage &msg);
		static void updateTableItem(QTableWidget &table, int row, int col, const QColor &color, const std::string &str);

		void sendKill(bool killed);
	};

	enum CombinedState {
		STOPPED,
		STARTED,
		STOPPING,

		ACTIVE,
		STANDBY,
		ERROR,
	};

	std::ostream &operator<<(std::ostream &out, CombinedState state);

	struct DisableSorting {
		DisableSorting(QTableWidget &table) : sorted(table.isSortingEnabled()), table(table) {
			table.setSortingEnabled(false);
		}

		~DisableSorting() {
			table.setSortingEnabled(sorted);
		}

		bool sorted;
		QTableWidget &table;
	};
}

#endif

