#include "LibSub/Worker/WorkerRunner.h"

using namespace subjugator;
using namespace boost;

WorkerRunner::WorkerRunner(Worker& worker, asio::io_service& io_service)
: worker(worker), timer(io_service), running(false) { }

void WorkerRunner::start() {
	if (running)
		return;

	running = true;
	prevtime = posix_time::microsec_clock::local_time();
	timer.expires_from_now(getDuration());
	timer.async_wait(bind(&WorkerRunner::tick, this, _1));

	worker.logger.log("Worker running");
}

void WorkerRunner::tick(const system::error_code& error) {
	if (error == asio::error::operation_aborted)
		return;

	posix_time::ptime curtime = posix_time::microsec_clock::local_time();
	double dt = (curtime - prevtime).total_microseconds() / 1.0E6;
	prevtime = curtime;

	worker.update(dt);

	curtime = posix_time::microsec_clock::local_time();
	posix_time::ptime expiretime = timer.expires_at() + getDuration();
	if (expiretime < curtime)
		expiretime = curtime;

	timer.expires_at(expiretime);
	timer.async_wait(bind(&WorkerRunner::tick, this, _1));
}

posix_time::time_duration WorkerRunner::getDuration() const {
	return posix_time::microseconds(1E6 / worker.getUpdateHz());
}

