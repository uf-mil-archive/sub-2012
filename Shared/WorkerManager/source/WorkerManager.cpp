#include "WorkerManager/WorkerManager.h"
#include <boost/system/system_error.hpp>

using namespace subjugator;
using namespace boost;
using namespace boost::system;
using namespace std;

WorkerManager::WorkerManager(const string &name, const std::string &procname, const vector<string> &args, const Options &opts)
: name(name), opts(opts), proc(procname, args), started(false), restarting(false), stoptimer(0), standbytimer(0), errortimer(0) { }

void WorkerManager::start() {
	if (started)
		return;

	proc.start();
	started = true;
	restarting = false;
}

void WorkerManager::restart() {
	if (!started) {
		start();
	} else {
		proc.stop();
		restarting = true;
	}
}

void WorkerManager::stop() {
	if (!started)
		return;

	proc.stop();
	started = false;
	restarting = false;
}

void WorkerManager::updateState(double dt) {
	if (state.code == State::UNINITIALIZED) // our state is active until we encounter an error, then stays at error
		state.code = State::ACTIVE;
	else if (state.code == State::ERROR)
		return;

	lastaction = NO_ACTION;

	try {
		proc.updateState();

		if (proc.getState() == Process::STOPPING) { // if process is stopping, kill it if it takes too long
			stoptimer += dt;
			if (stoptimer > opts.stopkilltime) {
				lastaction = KILL_STOP;
				proc.kill();
			}
			return;
		} else {
			stoptimer = 0;
		}

		if (!started) // otherwise, if we've not been started, we're done
			return;

		if (proc.getState() != Process::STARTED) { // process should be started, but it isn't currently
			if (opts.exitrestart || restarting) { // if we're configured to restart on exit, or if we're restarting the process ourselves
				if (!restarting)
					lastaction = RESTART_EXIT;

				proc.start(); // start the process
			} else {
				started = false;
			}

			return;
		}

		if (workerstate.code == State::STANDBY) { // process is running, but in standby
			standbytimer += dt;
			if (standbytimer > opts.standbyrestarttime) { // if it exceeds the standbytimer restart it
				lastaction = RESTART_STANDBY;
				restart();
			}
		} else {
			standbytimer = 0;
		}

		if (workerstate.code == State::ERROR) { // process is running, but in error
			errortimer += dt;
			if (errortimer > opts.errorrestarttime) { // if it exceeds the errortimer restart it
				lastaction = RESTART_ERROR;
				restart();
			}
		} else {
			errortimer = 0;
		}
	} catch (system_error &err) {
		state.code = State::ERROR;
		state.msg = "system_error while managing process: " + string(err.what());
	}
}

