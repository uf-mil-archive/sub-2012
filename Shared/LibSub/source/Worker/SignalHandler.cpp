#include "LibSub/Worker/SignalHandler.h"
#include <csignal>
#include <iostream>

using namespace subjugator;
using namespace std;

SignalHandler::SignalHandler(boost::asio::io_service &io, const ReconfigureCallback &reconfcallback)
: io(io), reconfcallback(reconfcallback), gotsigint(false) {
	globalptr = this;

	signal(SIGINT, globalptrAction);
	signal(SIGUSR1, globalptrAction);
}

SignalHandler::~SignalHandler() {
	globalptr = NULL;
	if (gotsigint)
		cout << endl; // puts terminal prompt correctly on a new line
}

void SignalHandler::signalReceived(int sig) {
	if (sig == SIGUSR1) {
		if (reconfcallback)
			io.post(reconfcallback);
	} else {
		gotsigint = true;
		io.stop();
		signal(SIGINT, SIG_DFL);
	}
}

void SignalHandler::globalptrAction(int sig) {
	if (globalptr)
		globalptr->signalReceived(sig);
}

SignalHandler *SignalHandler::globalptr;

