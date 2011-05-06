#include "HAL/RawWire.h"
#include <boost/bind.hpp>
#include <cassert>

using namespace subjugator;
using namespace boost;
using namespace std;

RawWire::RawWire(Transport *transport, PacketFormatterFactory packetformatterfactory)
: transport(transport), formatters(transport->getEndpointCount()) {
	transport->configureCallbacks(bind(&RawWire::transportReadCallback, this, _1, _2), bind(&RawWire::transportErrorCallback, this, _1, _2));

	for (int i=0; i<transport->getEndpointCount(); i++) {
		formatters.push_back(packetformatterfactory());
	}
}

void RawWire::configureCallbacks(ReadCallback readcallback, ErrorCallback errorcallback) {
	this->readcallback = readcallback;
	this->errorcallback = errorcallback;
}

void RawWire::transportReadCallback(int endnum, const ByteVec &bytes) {
	assert(endnum < formatters.size());

	vector<Packet> newpackets = formatters[endnum].parsePackets(bytes);

	if (readcallback) {
		for (vector<Packet>::iterator i = newpackets.begin(); i != newpackets.end(); ++i)
			readcallback(endnum, *i);
	}
}

void RawWire::transportErrorCallback(int endnum, const string &msg) {
	if (errorcallback)
		errorcallback(endnum, msg);
}

