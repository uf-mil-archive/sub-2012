#include "LibSub/Worker/SignalHandler.h"
#include <boost/bind.hpp>
#include <csignal>
#include <iostream>

using namespace subjugator;
using namespace boost;
using namespace std;

SignalHandler::SignalHandler(boost::asio::io_service &io, const ReconfigureCallback &reconfcallback)
: io(io), reconfcallback(reconfcallback), gotsigint(false) {
	assert(globalptr == NULL);
	globalptr = this;
}

SignalHandler::~SignalHandler() {
	stop();
	globalptr = NULL;
}

void SignalHandler::start() {
	signal(SIGINT, globalptrAction);
	signal(SIGUSR1, globalptrAction);
}

void SignalHandler::stop() {
	signal(SIGINT, SIG_DFL);
	signal(SIGUSR1, SIG_DFL);
}

void SignalHandler::globalptrAction(int sig) {
	if (globalptr)
		globalptr->signalReceived(sig);
}

void SignalHandler::signalReceived(int sig) {
	io.post(bind(&SignalHandler::ioCallback, this, sig));
	if (sig == SIGINT)
		stop(); // let the second SIGINT kill is immediately
}

void SignalHandler::ioCallback(int sig) {
	if (sig == SIGINT) {
		cerr << endl;
		io.stop();
	} else if (sig == SIGUSR1) {
		if (reconfcallback)
			reconfcallback();
	}
}

SignalHandler *SignalHandler::globalptr;

