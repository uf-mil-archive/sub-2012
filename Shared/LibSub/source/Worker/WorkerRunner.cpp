#include "LibSub/Worker/WorkerRunner.h"

using namespace subjugator;
using namespace boost;

WorkerRunner::WorkerRunner(Worker& worker, asio::io_service& io_service)
: worker(worker), timer_period(calculatePeriod(worker)), timer(io_service), running(false) { }

void WorkerRunner::start() {
	if (running)
		return;

	running = true;
	prevtime = posix_time::microsec_clock::local_time();
	timer.expires_from_now(timer_period);
	timer.async_wait(bind(&WorkerRunner::tick, this, _1));
}

void WorkerRunner::tick(const system::error_code& error) {
	if (error == asio::error::operation_aborted)
		return;

	posix_time::ptime curtime = posix_time::microsec_clock::local_time();
	posix_time::time_duration dt = curtime - prevtime;
	prevtime = curtime;

	if (!dt.is_negative() && dt < timer_period * 10) // protect against debugger or non-monotonic time
		worker.update(dt.total_microseconds() * 1E6);

	curtime = posix_time::microsec_clock::local_time();
	posix_time::ptime expiretime = timer.expires_at() + timer_period;
	if (expiretime < curtime)
		expiretime = curtime;

	timer.expires_at(expiretime);
	timer.async_wait(bind(&WorkerRunner::tick, this, _1));
}

posix_time::time_duration WorkerRunner::calculatePeriod(const Worker &worker) {
	return posix_time::microseconds(1E6 / worker.getUpdateHz());
}

