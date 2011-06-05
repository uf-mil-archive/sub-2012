#include "HAL/IOThread.h"
#include <cassert>

using namespace subjugator;
using namespace boost;
using namespace boost::asio;
using namespace std;

IOThread::IOThread() : running(false) { }

IOThread::~IOThread() {
	if (running)
		stop();
}

void IOThread::start() {
	assert(!running);
	iothread = thread(bind(&io_service::run, &ioservice)); // and start the io_service in its own thread
	running = true;
}

void IOThread::stop() {
	assert(running);
	ioservice.stop(); // tell the ioservice to stop its event loop
	iothread.join(); // wait for the iothread to terminate
	ioservice.reset(); // reset the io_service so it could be used again (somebody could call startIOThread a second time)
	running = false;
}

void IOThread::run(boost::function<void ()> callback) {
	ioservice.dispatch(callback);
}

