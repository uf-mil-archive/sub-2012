#include "LibSub/DDS/Conversions.h"
#include "SubControl/MainWindow.h"
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <map>

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
logreceiver(logtopic)
{
	ui.setupUi(this);
	ui.workerStatusTable->setHorizontalHeaderLabels(QStringList() << "En" << "Worker" << "Status" << "Message");
	ui.workerStatusTable->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);

	connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
	connect(ui.workerStatusTable, SIGNAL(cellChanged(int, int)), this, SLOT(cellChanged(int, int)));
	timer.start(100);
}

void MainWindow::update() {
	updating = true;
	updateWorkers();
	updateLog();
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

	cout << "Clicked " << workername << endl;
}

namespace {
	struct Entry {
		boost::optional<WorkerManagerProcessStatus> status;
		boost::optional<State> state;
	};
}

void MainWindow::updateWorkers() {
	ui.workerStatusTable->setRowCount(0);

	vector<shared_ptr<WorkerManagerStatusMessage> > statusmessages = statusreceiver.readAll();
	vector<shared_ptr<WorkerStateMessage> > statemessages = statereceiver.readAll();

	typedef map<std::string, Entry> EntryMap;
	EntryMap entrymap;

	for (unsigned int pos=0; pos<statusmessages.size(); ++pos) {
		const WorkerManagerStatusMessage &msg = *statusmessages[pos];
		string name = from_dds<string>(msg.workername);

		EntryMap::iterator i = entrymap.find(name);
		if (i == entrymap.end())
			i = entrymap.insert(make_pair(name, Entry())).first;

		i->second.status = msg.status;
	}

	for (unsigned int pos=0; pos<statemessages.size(); ++pos) {
		const WorkerStateMessage &msg = *statemessages[pos];
		string name = from_dds<string>(msg.worker);
		State state = from_dds<State>(msg.state);

		EntryMap::iterator i = entrymap.find(name);
		if (i == entrymap.end())
			i = entrymap.insert(make_pair(name, Entry())).first;

		i->second.state = state;
	}

	for (EntryMap::const_iterator i = entrymap.begin(); i != entrymap.end(); ++i) {
		const string &name = i->first;
		const Entry &entry = i->second;
		bool checkable=false;
		CombinedState state=STOPPED;
		string msg;

		if (entry.status) {
			checkable = true;
			state = static_cast<CombinedState>(*entry.status);
		}
		if (entry.state) {
			state = static_cast<CombinedState>(entry.state->code + 3);
			msg = entry.state->msg;
		}

		addWorker(name, state, checkable, msg);
	}
}

void MainWindow::addWorker(const std::string &name, CombinedState state, bool checkable, const std::string &msg) {
	QTableWidget &table = *ui.workerStatusTable;

	int row = table.rowCount();
	table.setRowCount(row+1);

	static const QColor statecolors[] = {
		Qt::white,
		Qt::white,
		Qt::white,

		Qt::green,
		Qt::yellow,
		Qt::red,

		Qt::white
	};
	const QColor &color = statecolors[state];

	QTableWidgetItem *checkitem = new QTableWidgetItem();
	checkitem->setCheckState((state >= ACTIVE || state == STARTED) ? Qt::Checked : Qt::Unchecked);
	if (!checkable)
		checkitem->setFlags(0);
	checkitem->setBackground(color);
	table.setItem(row, 0, checkitem);

	QTableWidgetItem *nameitem = new QTableWidgetItem(QString::fromStdString(name));
	nameitem->setBackground(color);
	table.setItem(row, 1, nameitem);

	QTableWidgetItem *codeitem = new QTableWidgetItem(QString::fromStdString(lexical_cast<string>(state)));
	codeitem->setBackground(color);
	table.setItem(row, 2, codeitem);

	QTableWidgetItem *msgitem = new QTableWidgetItem(QString::fromStdString(msg));
	msgitem->setBackground(color);
	table.setItem(row, 3, msgitem);
}

void MainWindow::updateLog() {
	while (true) {
		shared_ptr<WorkerLogMessage> msgptr = logreceiver.take();
		if (!msgptr)
			break;

		ui.logTextEdit->appendPlainText(QString::fromStdString(lexical_cast<string>(from_dds<WorkerLogEntry>(*msgptr))));
	}
}

ostream &subjugator::operator<<(ostream &out, CombinedState state) {
	static const char *strs[] = { "STOPPED", "STARTED", "STOPPING", "ACTIVE", "STANDBY", "ERROR" };
	out << strs[state];
	return out;
}

