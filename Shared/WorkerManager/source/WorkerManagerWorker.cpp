#include "WorkerManager/WorkerManagerWorker.h"
#include <boost/algorithm/string.hpp>
#include <limits>

using namespace subjugator;
using namespace boost;
using namespace boost::property_tree;
using namespace boost::algorithm;
using namespace std;

WorkerManagerWorker::WorkerManagerWorker(const string &suffix, const WorkerConfigLoader &configloader) :
	Worker("WorkerManager" + (suffix.size() ? "_" + suffix : ""), 2, configloader),
	commandmailbox(WorkerMailbox<Command>::Args()
	               .setName("command")
	               .setCallback(bind(&WorkerManagerWorker::commandSet, this, _1))),
	workerlist(*this)
{
	registerStateUpdater(workerlist);
}

void WorkerManagerWorker::initialize() {
	const ptree &workers = getConfig().get_child("workers");

	for (ptree::const_iterator i = workers.begin(); i != workers.end(); ++i) {
		const ptree &t = i->second;

		WorkerManager::Options opts;
		opts.exitrestart = t.get<bool>("exit_restart", true);
		opts.standbyrestarttime = t.get<double>("standby_restart_time", numeric_limits<double>::infinity());
		opts.errorrestarttime = t.get<double>("error_restart_time", numeric_limits<double>::infinity());
		opts.stopkilltime = t.get<double>("stop_kill_time", 5);

		string argsstr = t.get<string>("args", "");
		vector<string> args;
		split(args, argsstr, is_space(), token_compress_on);

		string name = t.get<string>("name");
		workerlist.addWorker(name, args, opts);
		statusupdatesignal.emit(StatusUpdate(name, Process::STOPPED));
	}
}

void WorkerManagerWorker::work(double dt) {
}

void WorkerManagerWorker::commandSet(const boost::optional<Command> &cmd) {
	if (!cmd)
		return;

	WorkerManager *wm = workerlist.getWorkerManager(cmd->workername);
	if (!wm)
		return;

	if (cmd->start) {
		logger.log("Starting " + wm->getName());
		wm->start();
		statusupdatesignal.emit(StatusUpdate(wm->getName(), Process::STARTED));
	} else {
		logger.log("Stopping " + wm->getName());
		wm->stop();
		statusupdatesignal.emit(StatusUpdate(wm->getName(), Process::STOPPING));
	}
}

void WorkerManagerWorker::processStateChanged(WorkerManager &wm) {
	const string &name = wm.getName();
	Process::State procstate = wm.getProcessState();

	stringstream buf;
	buf << "Worker " << name << " is " << procstate;
	if (procstate == Process::STOPPED && wm.getProcessTerminationReason().size()) {
		buf << " (" << wm.getProcessTerminationReason() << ")";
	}
	logger.log(buf.str());

	statusupdatesignal.emit(StatusUpdate(name, procstate));
}

void WorkerManagerWorker::workerManagerAction(WorkerManager &wm) {
	stringstream buf;
	buf << "Worker " << wm.getName();
	switch (wm.getLastAction()) {
		case WorkerManager::KILL_STOP:
			buf << " killed due to stop timeout";
			break;

		case WorkerManager::RESTART_EXIT:
			buf << " restarted due to an unexpected termination";
			break;

		case WorkerManager::RESTART_STANDBY:
			buf << " restarted due to extended time in standby state";
			break;

		case WorkerManager::RESTART_ERROR:
			buf << " restarted due to extended time in error state";
			break;

		default:
			break;
	}

	logger.log(buf.str());
}

