#include "HAL/HAL.h"
#include <boost/lexical_cast.hpp>
#include <fstream>
#include <stdexcept>

using namespace subjugator;
using namespace boost;
using namespace std;

HAL::HAL() { }

void HAL::addTransport(Transport *transport) {
	transports.push_back(transport);
}

void HAL::clearTransports() {
	transports.clear();
}

void HAL::loadAddressFile(const std::string &filename) {
	ifstream in(filename.c_str());
	if (!in)
		throw runtime_error("Failed to open address file " + filename);
	addrtable.read(in);
}

Endpoint *HAL::openEndpoint(int addr) {
	const AddressTable::Entry &entry = addrtable.getEntry(addr);

	TransportVec::iterator i;
	for (i = transports.begin(); i != transports.end(); ++i) {
		if (i->getName() == entry.protocol)
			break;
	}

	if (i == transports.end())
		throw runtime_error("HAL unable to open endpoint #" + lexical_cast<string>(addr) + " because no Transport provides required protocol " + entry.protocol);

	return i->makeEndpoint(entry.protoaddress, entry.params);
}

DataObjectEndpoint *HAL::openDataObjectEndpoint(int addr, DataObjectFormatter *dobjformat, PacketFormatter *packetformat) {
	return new DataObjectEndpoint(openEndpoint(addr), dobjformat, packetformat);
}

