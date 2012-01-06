#include "HAL/transport/UDPEndpoint.h"

using namespace subjugator;
using namespace boost;
using namespace boost::asio;
using namespace std;

UDPEndpoint::UDPEndpoint(const ip::udp::endpoint &endpoint, TransportCallbacks &callbacks)
: endpoint(endpoint), callbacks(callbacks), opened(false) { }

UDPEndpoint::~UDPEndpoint() {
	callbacks.endpointDeleted(this);
}

void UDPEndpoint::configureCallbacks(const ReadCallback &readcallback, const StateChangeCallback &statechangecallback) {
	this->readcallback = readcallback;
	this->statechangecallback = statechangecallback;
}

UDPEndpoint::State UDPEndpoint::getState() const {
	if (!opened)
		return CLOSED;
	else if (callbacks.getEndpointError().size())
		return ERROR;
	else
		return OPEN;
}

const string &UDPEndpoint::getErrorMessage() const {
	return callbacks.getEndpointError();
}

void UDPEndpoint::open() {
	assert(getState() == CLOSED);
	opened = true;
	callStateChangeCallback();
	callbacks.endpointOpened(this);
}

void UDPEndpoint::close() {
	assert(getState() != CLOSED);
	opened = false;
	callStateChangeCallback();
	callbacks.endpointClosed(this);
}

void UDPEndpoint::write(ByteVec::const_iterator begin, ByteVec::const_iterator end) {
	callbacks.endpointWrite(this, begin, end);
}

void UDPEndpoint::packetReceived(ByteVec::const_iterator begin, ByteVec::const_iterator end) {
	if (readcallback)
		readcallback(begin, end);
}

void UDPEndpoint::errorChanged() {
	callStateChangeCallback();
}

void UDPEndpoint::callStateChangeCallback() {
	if (statechangecallback)
		statechangecallback();
}

