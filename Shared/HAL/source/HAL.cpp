#include "HAL/HAL.h"
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <stdexcept>

using namespace subjugator;
using namespace boost::asio;
using namespace boost;
using namespace std;

BaseHAL::BaseHAL() { }

void BaseHAL::addTransport(Transport *transport) {
	transports.push_back(transport);
}

void BaseHAL::clearTransports() {
	transports.clear();
}

Endpoint *BaseHAL::makeEndpoint(const EndpointConfiguration &conf) {
	TransportVec::iterator i;
	for (i = transports.begin(); i != transports.end(); ++i) {
		if (i->getName() == conf.protocol)
			break;
	}

	if (i == transports.end())
		throw runtime_error("HAL unable to make endpoint because no Transport provides required protocol " + conf.protocol);

	return i->makeEndpoint(conf.protoaddress, conf.params);
}

DataObjectEndpoint *BaseHAL::makeDataObjectEndpoint(const EndpointConfiguration &conf, DataObjectFormatter *dobjformat, PacketFormatter *packetformat) {
	return new DataObjectEndpoint(makeEndpoint(conf), dobjformat, packetformat);
}

BaseHAL::EndpointConfiguration::EndpointConfiguration(const std::string &confstr) {
	static const regex entryreg("(\\w+)\\s*(\\S+)\\s*(.*)"); // parse the protocol, protocol address, and parameters

	smatch match;
	if (!regex_match(confstr, match, entryreg))
		throw runtime_error("Unable to parse endpoint configuration string '" + confstr + "'");

	protocol = match[1];
	protoaddress = match[2];

	const string &paramstr = match[3];
	if (paramstr.size() > 0) {
		static const regex paramreg("(\\w+)\\s*=\\s*(\\w+)\\s*");

		sregex_iterator regi(paramstr.begin(), paramstr.end(), paramreg);
		for (; regi != sregex_iterator(); ++regi) {
			const smatch &match = *regi;
			params.insert(make_pair(match[1], match[2]));
		}
	}
}

#include "HAL/transport/TCPTransport.h"
#include "HAL/transport/UDPTransport.h"
#include "HAL/transport/SerialTransport.h"
#include "HAL/transport/FileTransport.h"

HAL::HAL(io_service &io) {
	addTransport(new TCPTransport(io));
	addTransport(new UDPTransport(io));
	addTransport(new SerialTransport(io));
	addTransport(new FileTransport(io));
}
