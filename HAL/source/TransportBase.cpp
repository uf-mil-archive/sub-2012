#include "HAL/TransportBase.h"

using namespace subjugator;
using namespace boost;
using namespace boost::asio;
using namespace std;

void TransportBase::configureCallbacks(ReadCallback readcallback, ErrorCallback errorcallback) {
	this->readcallback = readcallback;
	this->errorcallback = errorcallback;
}

void ASIOTransportBase::startIOThread() {
	if (iothread_running)
		return;

	iothread = thread(bind(&io_service::run, &ioservice)); // and start the io_service in its own thread
	iothread_running = true;
}

void ASIOTransportBase::stopIOThread() {
	if (!iothread_running)
		return;

	ioservice.stop(); // tell the ioservice to stop its event loop
	iothread.join(); // wait for the iothread to terminate
	ioservice.reset(); // reset the io_service so it could be used again (somebody could call startIOThread a second time)
	iothread_running = false;
}

void ASIOTransportBase::runCallbackOnIOThread(boost::function<void ()> callback) {
	ioservice.dispatch(callback);
}

