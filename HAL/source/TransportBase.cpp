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
	iothread = thread(bind(&io_service::run, &ioservice)); // and start the io_service in its own thread
}

void ASIOTransportBase::stopIOThread() {
	ioservice.stop(); // tell the ioservice to stop its event loop
	iothread.join(); // wait for the iothread to terminate, so that we can safely destroy the objects that it uses
}

void ASIOTransportBase::runCallbackOnIOThread(boost::function<void ()> callback) {
	ioservice.dispatch(callback);
}

