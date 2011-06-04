#include "HAL/transport/UDPEndpoint.h"

using namespace subjugator;
using namespace boost;
using namespace boost::asio;
using namespace std;

UDPEndpoint::UDPEndpoint(const ip::udp::endpoint &endpoint, IOThread &iothread, TransportCallbacks &callbacks)
: BaseEndpoint(iothread), endpoint(endpoint), callbacks(callbacks) { }

UDPEndpoint::~UDPEndpoint() {
	callbacks.endpointDeleted(this);
}

void UDPEndpoint::open() {
	assert(getState() == CLOSED);
	setState(OPEN);
}

void UDPEndpoint::close() {
	setState(CLOSED);
}

void UDPEndpoint::write(ByteVec::const_iterator begin, ByteVec::const_iterator end) {
	callbacks.endpointWrite(this, begin, end);
}

void UDPEndpoint::packetReceived(ByteVec::const_iterator begin, ByteVec::const_iterator end) {
	callReadCallback(begin, end);
}

