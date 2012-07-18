#include "DDS/Conversions.h"
#include "SubControl/MainWindow.h"
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <boost/algorithm/string.hpp>
#include <map>
#include <vector>
#include <algorithm>
#include <QFileDialog>
#include <fstream>
#include <unistd.h>

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
	interactioncommandtopic(part, "InteractionCommand", TopicQOS::RELIABLE),
	interactioncommandsender(interactioncommandtopic),
	interactioncommandreceiver(interactioncommandtopic),
	interactionstatustopic(part, "InteractionStatus", TopicQOS::PERSISTENT | TopicQOS::EXCLUSIVE),
	interactionstatusreceiver(interactionstatustopic),
	interactionoutputtopic(part, "InteractionOutput", TopicQOS::DEEP_PERSISTENT),
	interactionoutputreceiver(interactionoutputtopic),
	availablemissionstopic(part, "AvailableMissions", TopicQOS::PERSISTENT | TopicQOS::EXCLUSIVE),
	availablemissionsreceiver(availablemissionstopic),
	missionlisttopic(part, "MissionList", TopicQOS::PERSISTENT),
	missionlistreceiver(missionlisttopic),
	missioncommandtopic(part, "MissionCommand", TopicQOS::RELIABLE),
	missioncommandsender(missioncommandtopic),
	missionstatetopic(part, "MissionState", TopicQOS::PERSISTENT),
	missionstatereceiver(missionstatetopic),
	stats(part),
	commandhistorypos(0)
{
	ui.setupUi(this);
	ui.workerStatusTable->setHorizontalHeaderLabels(QStringList() << "En" << "Worker" << "Status" << "Message");
	ui.workerStatusTable->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);

	ui.killStatusTable->setHorizontalHeaderLabels(QStringList() << "Name" << "Status" << "Description");
	ui.killStatusTable->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);

	ui.unkillButton->setVisible(false);
	ui.interactCommandTextEdit->installEventFilter(&filter);

	connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
	connect(ui.workerStatusTable, SIGNAL(cellChanged(int, int)), this, SLOT(cellChanged(int, int)));
	connect(ui.killButton, SIGNAL(clicked()), this, SLOT(killClicked()));
	connect(ui.unkillButton, SIGNAL(clicked()), this, SLOT(unkillClicked()));
	connect(ui.runButton, SIGNAL(clicked()), this, SLOT(unkillClicked()));
	connect(ui.interactRunButton, SIGNAL(clicked()), this, SLOT(interactRunClicked()));
	connect(ui.interactStopButton, SIGNAL(clicked()), this, SLOT(interactStopClicked()));
	connect(&filter, SIGNAL(sendCommandTyped()), this, SLOT(interactRunClicked()));
	connect(&filter, SIGNAL(recallCommandTyped()), this, SLOT(interactRecall()));
	connect(&filter, SIGNAL(unrecallCommandTyped()), this, SLOT(interactUnrecall()));
	connect(ui.missionAddButton, SIGNAL(clicked()), this, SLOT(missionListAdd()));
	connect(ui.missionRemoveButton, SIGNAL(clicked()), this, SLOT(missionListRemove()));
	connect(ui.missionStartButton, SIGNAL(clicked()), this, SLOT(missionStart()));
	connect(ui.missionStopButton, SIGNAL(clicked()), this, SLOT(missionStop()));
	connect(ui.missionLoadButton, SIGNAL(clicked()), this, SLOT(missionLoad()));
	connect(ui.missionSaveButton, SIGNAL(clicked()), this, SLOT(missionSave()));

	timer.start(100);
}

void MainWindow::update() {
	updating = true;
 	updateWorkers();
	updateKills();
	updateLog();
	updateStats();
	updateInteract();
	updateInteractOutput();
	updateAvailableMissions();
	updateMissionLists();
	updateMissionList();
	updateMissionState();
	updating = false;
}

void MainWindow::cellChanged(int row, int col) {
	if (updating)
		return;

	string workername = ui.workerStatusTable->item(row, 1)->text().toStdString();
	bool start = ui.workerStatusTable->item(row, 0)->checkState() == Qt::Checked;

	MessageWrapper<WorkerManagerCommandMessage> cmd;
	to_dds(cmd->workername, workername);
	to_dds(cmd->start, start);
	commandsender.send(*cmd);
}

void MainWindow::killClicked() {
	sendKill(true);
}

void MainWindow::unkillClicked() {
	sendKill(false);
}

void MainWindow::interactRunClicked() {
	QString cmd = ui.interactCommandTextEdit->toPlainText();
	ui.interactCommandTextEdit->clear();

	MessageWrapper<InteractionCommandMessage> msg;
	to_dds(msg->cmd, cmd.toStdString());
	to_dds(msg->stop, false);
	interactioncommandsender.send(*msg);
	commandhistorypos = 0;
	commandhistory.push_back(cmd.toStdString());
}

void MainWindow::interactStopClicked() {
	MessageWrapper<InteractionCommandMessage> msg;
	to_dds(msg->cmd, "");
	to_dds(msg->stop, true);
	interactioncommandsender.send(*msg);
}

void MainWindow::interactRecall() {
	string cmd;
	if (commandhistorypos >= commandhistory.size()) {
		commandhistorypos = 0;
	} else {
		commandhistorypos++;
		cmd = commandhistory[commandhistory.size()-commandhistorypos];
	}

	ui.interactCommandTextEdit->setPlainText(cmd.c_str());
}

void MainWindow::interactUnrecall() {
	string cmd;
	if (commandhistorypos <= 1) {
		commandhistorypos = 0;
	} else {
		commandhistorypos--;
		cmd = commandhistory[commandhistory.size()-commandhistorypos];
	}

	ui.interactCommandTextEdit->setPlainText(cmd.c_str());
}

void MainWindow::missionListAdd() {
	QListWidgetItem *item = ui.availableMissionsList->currentItem();
	if (!item)
		return;
	string mission = item->text().toStdString();
	string list = ui.missionListCombo->currentText().toStdString();

	MessageWrapper<MissionCommandMessage> msg;
	to_dds(msg->type, MISSIONCOMMANDTYPE_ADD_MISSION);
	to_dds(msg->pos, ui.missionList->currentRow());
	to_dds(msg->list, list);
	to_dds(msg->mission, mission);
	missioncommandsender.send(*msg);
}

void MainWindow::missionListRemove() {
	if (ui.missionList->currentRow() == -1)
		return;
	string list = ui.missionListCombo->currentText().toStdString();

	MessageWrapper<MissionCommandMessage> msg;
	to_dds(msg->type, MISSIONCOMMANDTYPE_REMOVE_MISSION);
	to_dds(msg->pos, ui.missionList->currentRow());
	to_dds(msg->list, list);
	to_dds(msg->mission, "");
	missioncommandsender.send(*msg);
}

void MainWindow::missionStart() {
	MessageWrapper<MissionCommandMessage> msg;
	to_dds(msg->type, MISSIONCOMMANDTYPE_START);
	to_dds(msg->pos, -1);
	to_dds(msg->list, "");
	to_dds(msg->mission, "");
	missioncommandsender.send(*msg);
}

void MainWindow::missionStop() {
	MessageWrapper<MissionCommandMessage> msg;
	to_dds(msg->type, MISSIONCOMMANDTYPE_STOP);
	to_dds(msg->pos, -1);
	to_dds(msg->list, "");
	to_dds(msg->mission, "");
	missioncommandsender.send(*msg);
}

void MainWindow::missionSave() {
	QString filename = QFileDialog::getSaveFileName(this);
	ofstream out(filename.toStdString().c_str());

	vector<boost::shared_ptr<MissionListMessage> > msgs = missionlistreceiver.readAll();
	for (unsigned int msgnum=0; msgnum < msgs.size(); msgnum++) {
		boost::shared_ptr<MissionListMessage> &msg = msgs[msgnum];
		for (int i=0; i<msg->missions.length(); i++) {
			out << msg->name << "," << msg->missions[i] << endl;
		}
	}
}

void MainWindow::missionLoad() {
	QString filename = QFileDialog::getOpenFileName(this);
	ifstream in(filename.toStdString().c_str());

	while (true) {
		string line;
		getline(in, line);
		if (!in)
			break;

		vector<string> parts;
		split(parts, line, is_any_of(","), token_compress_on);
		assert(parts.size() == 2);
		const string &list = parts[0];
		const string &mission = parts[1];

		cout << "list: " << list << " mission: " << mission << endl;
		MessageWrapper<MissionCommandMessage> msg;
		to_dds(msg->type, MISSIONCOMMANDTYPE_ADD_MISSION);
		to_dds(msg->pos, ui.missionList->currentRow());
		to_dds(msg->list, list);
		to_dds(msg->mission, mission);
		missioncommandsender.send(*msg);

		usleep(10*1e3); // DDS...
	}
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
		ui.thrusterLFORLabel->setText(QString("%1%").arg(data.efforts.lfor*100, 0, 'f', 2));
		ui.thrusterRFORLabel->setText(QString("%1%").arg(data.efforts.rfor*100, 0, 'f', 2));
		ui.thrusterFSLabel->setText(QString("%1%").arg(data.efforts.fs*100, 0, 'f', 2));
		ui.thrusterRSLabel->setText(QString("%1%").arg(data.efforts.rs*100, 0, 'f', 2));
		ui.thrusterFLVLabel->setText(QString("%1%").arg(data.efforts.flv*100, 0, 'f', 2));
		ui.thrusterFRVLabel->setText(QString("%1%").arg(data.efforts.frv*100, 0, 'f', 2));
		ui.thrusterRLVLabel->setText(QString("%1%").arg(data.efforts.rlv*100, 0, 'f', 2));
		ui.thrusterRRVLabel->setText(QString("%1%").arg(data.efforts.rrv*100, 0, 'f', 2));
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

	if (data.hydrophone.avail) {
		ui.headingLabel->setText(QString("%1 deg").arg(data.hydrophone.heading/M_PI*180, 0, 'f', 0));
		ui.declinationLabel->setText(QString("%1 deg").arg(data.hydrophone.declination/M_PI*180, 0, 'f', 0));
		ui.distanceLabel->setText(QString("%1 m").arg(data.hydrophone.dist, 0, 'f', 0));
		ui.frequencyLabel->setText(QString("%1 khz").arg(data.hydrophone.freq/1000, 0, 'f', 0));
	} else {
		ui.headingLabel->setText(QString("Unavailable"));
		ui.declinationLabel->setText(QString("Unavailable"));
		ui.distanceLabel->setText(QString("Unavailable"));
		ui.frequencyLabel->setText(QString("Unavailable"));
	}
}

void MainWindow::updateInteract() {
	boost::shared_ptr<InteractionStatusMessage> msg = interactionstatusreceiver.take();
	if (!msg)
		return;

	const char *strs[] = {"Done", "Running", "Error"};
	ui.interactStatusLabel->setText(QString(strs[msg->status]));
}

void MainWindow::updateInteractOutput() {
	boost::shared_ptr<InteractionOutputMessage> msg;
	while (msg = interactionoutputreceiver.take()) {
		static const char *colors[] = {"black", "black", "blue", "red"};

		QString data = msg->data;
		data.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;");

		ostringstream out;
		out << "<span style=\"";
		out << "color: " << colors[msg->type] << ";";
		out << "white-space: pre;";
		if (msg->type == IOT_STATUS)
			out << "font-style: italic;";
		out << "\">";
		out << data.toStdString();
		out << "</span>";
		if (msg->type == IOT_STATUS)
			out << "<br />";

		ui.interactOutputTextEdit->appendHtml(QString(out.str().c_str()));
		ui.interactOutputTextEdit->moveCursor(QTextCursor::End);
		ui.interactOutputTextEdit->ensureCursorVisible();
	}
}

void MainWindow::updateAvailableMissions() {
	boost::shared_ptr<AvailableMissionsMessage> msg;
	if (!(msg = availablemissionsreceiver.take()))
		return;

	vector<string> names;
	ui.availableMissionsList->clear();
	for (int i=0; i<msg->missions.length(); i++) {
		names.push_back(msg->missions[i]);
	}

	sort(names.begin(), names.end());
	for (unsigned int i=0; i<names.size(); i++) {
		ui.availableMissionsList->addItem(names[i].c_str());
	}
}

static bool by_name_desc(const boost::shared_ptr<MissionListMessage> &a, const boost::shared_ptr<MissionListMessage> &b) {
	return strcmp(a->name, b->name) > 0;
}

void MainWindow::updateMissionLists() {
	vector<boost::shared_ptr<MissionListMessage> > msgs = missionlistreceiver.readAll();
	sort(msgs.begin(), msgs.end(), by_name_desc);

	bool changed;
	if (msgs.size() != (unsigned int)ui.missionListCombo->count()) {
		changed = true;
	} else {
		changed = false;
		for (unsigned int i=0; i<msgs.size(); i++) {
			if (ui.missionListCombo->itemText(i) != QString(msgs[i]->name)) {
				changed = true;
				break;
			}
		}
	}

	if (!changed)
		return;

	ui.missionListCombo->clear();
	for (unsigned int i=0; i<msgs.size(); i++) {
		ui.missionListCombo->addItem(msgs[i]->name);
	}
}

void MainWindow::updateMissionList() {
	string curmission = ui.missionListCombo->currentText().toStdString();

	vector<boost::shared_ptr<MissionListMessage> > msgs = missionlistreceiver.readAll();
	boost::shared_ptr<MissionListMessage> curmsg;

	for (unsigned int i=0; i<msgs.size(); ++i) {
		if (string(msgs[i]->name) == curmission) {
			curmsg = msgs[i];
			break;
		}
	}

	if (!curmsg)
		return;

	bool changed=false;

	if (curmsg->missions.length() != ui.missionList->count()) {
		changed = true;
	} else {
		for (int i=0; i<curmsg->missions.length(); ++i) {
			if (QString(curmsg->missions[i]) != ui.missionList->item(i)->text()) {
				changed = true;
				break;
			}
		}
	}

	if (!changed)
		return;

	ui.missionList->clear();
	for (int i=0; i<curmsg->missions.length(); ++i) {
		ui.missionList->addItem(curmsg->missions[i]);
	}
}

void MainWindow::updateMissionState() {
	boost::shared_ptr<MissionStateMessage> msg = missionstatereceiver.read();
	if (msg && msg->running) {
		ui.missionStateLabel->setText(msg->state);
	} else {
		ui.missionStateLabel->setText("none");
	}
}

void MainWindow::updateTableItem(QTableWidget &table, int row, int col, const QColor &color, const std::string &str) {
	QTableWidgetItem *item = new QTableWidgetItem(QString::fromStdString(str));
	item->setBackground(color);
	table.setItem(row, col, item);
}

void MainWindow::sendKill(bool killed) {
	MessageWrapper<WorkerKillMessage> msg;
	to_dds(msg->name, "SubControl");
	to_dds(msg->desc, "SubControl GUI kill button");
	to_dds(msg->killed, killed);
	killsender.send(*msg);
}

ostream &subjugator::operator<<(ostream &out, CombinedState state) {
	static const char *strs[] = { "STOPPED", "STARTED", "STOPPING", "ACTIVE", "STANDBY", "ERROR" };
	out << strs[state];
	return out;
}

bool InteractTextEditFilter::eventFilter(QObject *obj, QEvent *event) {
	 if (event->type() == QEvent::KeyPress) {
         QKeyEvent &evt = *static_cast<QKeyEvent *>(event);
         if (evt.modifiers().testFlag(Qt::ControlModifier) && evt.key() == Qt::Key_Return) {
	         emit sendCommandTyped();
	         return true;
         } else if (evt.modifiers().testFlag(Qt::ControlModifier) && evt.key() == Qt::Key_Up) {
	         emit recallCommandTyped();
	         return true;
         } else if (evt.modifiers().testFlag(Qt::ControlModifier) && evt.key() == Qt::Key_Down) {
	         emit unrecallCommandTyped();
	         return true;
         }
	 }
	 return QObject::eventFilter(obj, event);
}
