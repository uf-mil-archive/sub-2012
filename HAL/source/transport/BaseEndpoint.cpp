#include "HAL/transport/BaseEndpoint.h"
#include <boost/bind.hpp>

using namespace subjugator;
using namespace boost;
using namespace std;

BaseEndpoint::BaseEndpoint(IOThread &iothread) : iothread(iothread), state(CLOSED) { }

void BaseEndpoint::configureCallbacks(const ReadCallback &readcallback, const StateChangeCallback &statechangecallback) {
	this->readcallback = readcallback;
	this->statechangecallback = statechangecallback;
}

void BaseEndpoint::setState(State state, const std::string &errmsg) {
	this->state = state;
	this->errmsg = errmsg;

	if (statechangecallback)
		iothread.run(statechangecallback);
}

void BaseEndpoint::callReadCallback(ByteVec::const_iterator begin, ByteVec::const_iterator end) {
	if (readcallback)
		readcallback(begin, end);
}

