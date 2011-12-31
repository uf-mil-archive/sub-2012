#include "HAL/format/DataObjectEndpoint.h"
#include <boost/bind.hpp>

using namespace subjugator;
using namespace boost;
using namespace std;

DataObjectEndpoint::DataObjectEndpoint(Endpoint *endpoint, DataObjectFormatter *dobjformat, PacketFormatter *packetformat)
: endpoint(endpoint), dobjformat(dobjformat), packetformat(packetformat) {
	endpoint->configureCallbacks(boost::bind(&DataObjectEndpoint::endpointReadCallback, this, _1, _2), boost::bind(&DataObjectEndpoint::endpointStateChangeCallback, this));
}

void DataObjectEndpoint::configureCallbacks(const ReadCallback &readcallback, const StateChangeCallback &statechangecallback) {
	this->readcallback = readcallback;
	this->statechangecallback = statechangecallback;
}

void DataObjectEndpoint::write(const DataObject &dobj) {
	Packet packet = dobjformat->toPacket(dobj);
	ByteVec bytes = packetformat->formatPacket(packet);

	endpoint->write(bytes.begin(), bytes.end());
}

void DataObjectEndpoint::endpointReadCallback(ByteVec::const_iterator begin, ByteVec::const_iterator end) {
	vector<Packet> packets = packetformat->parsePackets(begin, end); // parse packets from the new data

	for (vector<Packet>::iterator i = packets.begin(); i != packets.end(); ++i) { // for each packet
		auto_ptr<DataObject> dobj(dobjformat->toDataObject(*i)); // attempt to convert it to a data object
		if (dobj.get()) { // if we succeeded
			if (readcallback)
				readcallback(dobj); // call our read callback (which can take optionally take ownership of the dataobject, otherwise auto_ptr will delete it)
		}
	}
}

void DataObjectEndpoint::endpointStateChangeCallback() {
	if (statechangecallback)
		statechangecallback();
}
