#include "DDS/Conversions.h"
#include "SubControl/MainWindow.h"
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <map>
#include <vector>

using namespace subjugator;
using namespace boost;
using namespace std;

MainWindow::MainWindow() :
	timer(this),
	updating(false),
	statustopic(part, "WorkerManagerStatus", TopicQOS::PERSISTENT | TopicQOS::LIVELINESS),
	statusreceiver(statustopic),
	commandtopic(part, "WorkerManagerCommand", TopicQOS::RELIABLE),
	commandsender(commandtopic),
	statetopic(part, "WorkerState", TopicQOS::PERSISTENT | TopicQOS::LIVELINESS),
	statereceiver(statetopic),
	logtopic(part, "WorkerLog", TopicQOS::DEEP_PERSISTENT),
	logreceiver(logtopic),
	killtopic(part, "WorkerKill", TopicQOS::PERSISTENT | TopicQOS::LIVELINESS),
	killreceiver(killtopic),
	killsender(killtopic),
	stats(part)
{
	ui.setupUi(this);
	ui.workerStatusTable->setHorizontalHeaderLabels(QStringList() << "En" << "Worker" << "Status" << "Message");
	ui.workerStatusTable->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);

	ui.killStatusTable->setHorizontalHeaderLabels(QStringList() << "Name" << "Status" << "Description");
	ui.killStatusTable->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);

	ui.unkillButton->setVisible(false);

	connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
	connect(ui.workerStatusTable, SIGNAL(cellChanged(int, int)), this, SLOT(cellChanged(int, int)));
	connect(ui.killButton, SIGNAL(clicked()), this, SLOT(killClicked()));
	connect(ui.unkillButton, SIGNAL(clicked()), this, SLOT(unkillClicked()));
	connect(ui.runButton, SIGNAL(clicked()), this, SLOT(unkillClicked()));
	timer.start(100);
}

void MainWindow::update() {
	updating = true;
	updateWorkers();
	updateKills();
	updateLog();
	updateStats();
	updating = false;
}

void MainWindow::cellChanged(int row, int col) {
	if (updating)
		return;

	string workername = ui.workerStatusTable->item(row, 1)->text().toStdString();
	bool start = ui.workerStatusTable->item(row, 0)->checkState() == Qt::Checked;

	WorkerManagerCommandMessage cmd;
	to_dds(cmd.workername, workername);
	to_dds(cmd.start, start);
	commandsender.send(cmd);
}

void MainWindow::killClicked() {
	sendKill(true);
}

void MainWindow::unkillClicked() {
	sendKill(false);
}

void MainWindow::updateWorkers() {
	vector<boost::shared_ptr<WorkerManagerStatusMessage> > statusmessages = statusreceiver.readAll();
	vector<boost::shared_ptr<WorkerStateMessage> > statemessages = statereceiver.readAll();

	typedef pair<optional<WorkerManagerProcessStatus>, optional<State> > Entry;
	typedef map<string, Entry> EntryMap;
	EntryMap entrymap;

	for (unsigned int pos=0; pos<statusmessages.size(); ++pos) {
		const WorkerManagerStatusMessage &msg = *statusmessages[pos];
		string name = from_dds<string>(msg.workername);

		entrymap[name].first = msg.status;
	}

	for (unsigned int pos=0; pos<statemessages.size(); ++pos) {
		const WorkerStateMessage &msg = *statemessages[pos];
		string name = from_dds<string>(msg.worker);
		State state = from_dds<State>(msg.state);

		entrymap[name].second = state;
	}

	QTableWidget &table = *ui.workerStatusTable;
	DisableSorting disablesorting(table);

	for (int row=0; row<table.rowCount();) {
		string name = table.item(row, 1)->text().toStdString();

		EntryMap::iterator i = entrymap.find(name);
		if (i != entrymap.end()) {
			const Entry &entry = i->second;
			updateWorker(row, name, entry.first, entry.second);
			row++;
			entrymap.erase(i);
		} else {
			table.removeRow(row);
		}
	}

	for (EntryMap::iterator i = entrymap.begin(); i != entrymap.end(); ++i) {
		int row = table.rowCount();
		table.setRowCount(row+1);

		const Entry &entry = i->second;
		updateWorker(row, i->first, entry.first, entry.second);
	}
}

void MainWindow::updateWorker(int row, const std::string &name, const boost::optional<WorkerManagerProcessStatus> &status, const boost::optional<State> &state) {
	bool checkable=false;
	CombinedState cstate=STOPPED;
	string msg;

	if (status) {
		checkable = true;
		cstate = static_cast<CombinedState>(*status);
	}
	if (state) {
		cstate = static_cast<CombinedState>(state->code + 3);
		msg = state->msg;
	}

	QTableWidget &table = *ui.workerStatusTable;
	DisableSorting disablesorting(table);
	static const QColor statecolors[] = {
		Qt::white,
		Qt::white,
		Qt::white,

		Qt::green,
		Qt::yellow,
		Qt::red,

		Qt::white
	};
	const QColor &color = statecolors[cstate];

	QTableWidgetItem *checkitem = new QTableWidgetItem();
	checkitem->setCheckState((cstate >= ACTIVE || cstate == STARTED) ? Qt::Checked : Qt::Unchecked);
	if (!checkable)
		checkitem->setFlags(0);
	checkitem->setBackground(color);
	table.setItem(row, 0, checkitem);

	updateTableItem(table, row, 1, color, name);
	updateTableItem(table, row, 2, color, lexical_cast<string>(cstate));
	updateTableItem(table, row, 3, color, msg);
}

void MainWindow::updateKills() {
	typedef map<string, boost::shared_ptr<WorkerKillMessage> > KillMap;
	KillMap killmap;

	vector<boost::shared_ptr<WorkerKillMessage> > killmessages = killreceiver.readAll();
	for (unsigned int i=0; i<killmessages.size(); i++) {
		killmap[killmessages[i]->name] = killmessages[i];
	}

	QTableWidget &table = *ui.killStatusTable;
	for (int row=0; row<table.rowCount();) {
		string name = table.item(row, 0)->text().toStdString();
		KillMap::iterator i = killmap.find(name);
		if (i != killmap.end()) {
			updateKill(row, *i->second);
			row++;
			killmap.erase(i);
		} else {
			table.removeRow(row);
		}
	}

	for (KillMap::iterator i = killmap.begin(); i != killmap.end(); ++i) {
		int row = table.rowCount();
		table.setRowCount(row+1);
		updateKill(row, *i->second);
	}

	bool otherkilled = false; // somebody other than us has the sub killed
	bool selfkilled = false; // we're killing the sub
	for (unsigned int i=0; i<killmessages.size(); ++i) {
		if (killmessages[i]->killed) {
			if (string(killmessages[i]->name) != "SubControl")
				otherkilled = true;
			else
				selfkilled = true;
		}
	}

	ui.unkillButton->setVisible(otherkilled);
	ui.runButton->setVisible(!otherkilled);

	ostringstream text;
	text << "Sub status: ";
	if (selfkilled) {
		text << "<span style=\"color: red\">killed</span>";
	} else if (otherkilled) {
		text << "<span style=\"color: darkgoldenrod\">killed</span>";
	} else {
		text << "<span style=\"color: green\">running</span>";
	}
	ui.killStatusLabel->setText(QString::fromStdString(text.str()));
}

void MainWindow::updateKill(int row, const WorkerKillMessage &msg) {
	QTableWidget &table = *ui.killStatusTable;

	QColor color(msg.killed ? Qt::red : Qt::green);

	updateTableItem(table, row, 0, color, msg.name);
	updateTableItem(table, row, 1, color, msg.killed ? "Killed" : "Unkilled");
	updateTableItem(table, row, 2, color, msg.desc);
}

void MainWindow::updateLog() {
	while (true) {
		boost::shared_ptr<WorkerLogMessage> msgptr = logreceiver.take();
		if (!msgptr)
			break;

		ui.logTextEdit->appendPlainText(QString::fromStdString(lexical_cast<string>(from_dds<WorkerLogEntry>(*msgptr))));
	}
}

void MainWindow::updateStats() {
	Stats::Data data = stats.getData();

	if (data.rails.avail) {
		ui.power16Label->setText(QString("%1A @ %2V").arg(data.rails.r16.current, 0, 'f', 2).arg(data.rails.r16.voltage, 0, 'f', 2));
		ui.power32Label->setText(QString("%1A @ %2V").arg(data.rails.r32.current, 0, 'f', 2).arg(data.rails.r32.voltage, 0, 'f', 2));
	} else {
		ui.power16Label->setText("Unavailable");
		ui.power32Label->setText("Unavailable");
	}

	if (data.lposvss.avail) {
		ui.LPOSVSSXLabel->setText(QString("%1 m").arg(data.lposvss.x, 0, 'f', 2));
		ui.LPOSVSSYLabel->setText(QString("%1 m").arg(data.lposvss.y, 0, 'f', 2));
		ui.LPOSVSSZLabel->setText(QString("%1 m").arg(data.lposvss.z, 0, 'f', 2));
		ui.LPOSVSSRollLabel->setText(QString("%1 deg").arg(data.lposvss.R*180/M_PI, 0, 'f', 2));
		ui.LPOSVSSPitchLabel->setText(QString("%1 deg").arg(data.lposvss.P*180/M_PI, 0, 'f', 2));
		ui.LPOSVSSYawLabel->setText(QString("%1 deg").arg(data.lposvss.Y*180/M_PI, 0, 'f', 2));
	} else {
		ui.LPOSVSSXLabel->setText(QString("Unavailable"));
		ui.LPOSVSSYLabel->setText(QString("Unavailable"));
		ui.LPOSVSSZLabel->setText(QString("Unavailable"));
		ui.LPOSVSSRollLabel->setText(QString("Unavailable"));
		ui.LPOSVSSPitchLabel->setText(QString("Unavailable"));
		ui.LPOSVSSYawLabel->setText(QString("Unavailable"));
	}

	if (data.efforts.avail) {
		ui.thrusterLFORLabel->setText(QString("%1%").arg(data.efforts.lfor, 0, 'f', 2));
		ui.thrusterRFORLabel->setText(QString("%1%").arg(data.efforts.rfor, 0, 'f', 2));
		ui.thrusterFSLabel->setText(QString("%1%").arg(data.efforts.fs, 0, 'f', 2));
		ui.thrusterRSLabel->setText(QString("%1%").arg(data.efforts.rs, 0, 'f', 2));
		ui.thrusterFLVLabel->setText(QString("%1%").arg(data.efforts.flv, 0, 'f', 2));
		ui.thrusterFRVLabel->setText(QString("%1%").arg(data.efforts.frv, 0, 'f', 2));
		ui.thrusterRLVLabel->setText(QString("%1%").arg(data.efforts.rlv, 0, 'f', 2));
		ui.thrusterRRVLabel->setText(QString("%1%").arg(data.efforts.rrv, 0, 'f', 2));
	} else {
		ui.thrusterLFORLabel->setText(QString("Unavailable"));
		ui.thrusterRFORLabel->setText(QString("Unavailable"));
		ui.thrusterFSLabel->setText(QString("Unavailable"));
		ui.thrusterRSLabel->setText(QString("Unavailable"));
		ui.thrusterFLVLabel->setText(QString("Unavailable"));
		ui.thrusterFRVLabel->setText(QString("Unavailable"));
		ui.thrusterRLVLabel->setText(QString("Unavailable"));
		ui.thrusterRRVLabel->setText(QString("Unavailable"));
	}
}

void MainWindow::updateTableItem(QTableWidget &table, int row, int col, const QColor &color, const std::string &str) {
	QTableWidgetItem *item = new QTableWidgetItem(QString::fromStdString(str));
	item->setBackground(color);
	table.setItem(row, col, item);
}

void MainWindow::sendKill(bool killed) {
	WorkerKillMessage msg;
	msg.name = const_cast<char *>("SubControl");
	msg.desc = const_cast<char *>("SubControl GUI kill button");
	msg.killed = killed;
	killsender.send(msg);
}

ostream &subjugator::operator<<(ostream &out, CombinedState state) {
	static const char *strs[] = { "STOPPED", "STARTED", "STOPPING", "ACTIVE", "STANDBY", "ERROR" };
	out << strs[state];
	return out;
}

