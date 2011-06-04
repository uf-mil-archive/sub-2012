#include "HAL/transport/TCPTransport.h"
#include "HAL/transport/TCPEndpoint.h"
#include <boost/lexical_cast.hpp>
#include <boost/asio.hpp>
#include <boost/regex.hpp>
#include <cassert>
#include <stdexcept>

using namespace subjugator;
using namespace boost;
using namespace boost::asio;
using namespace std;

TCPTransport::TCPTransport(IOThread &iothread) : iothread(iothread) { }

const string &TCPTransport::getName() const {
	static const string name = "tcp";
	return name;
}

Endpoint *TCPTransport::makeEndpoint(const std::string &address, std::map<std::string, std::string> params) {
	static const regex ipreg("(\\d+\\.\\d+\\.\\d+\\.\\d+):(\\d+)");
	smatch match;
	if (!regex_match(address, match, ipreg))
		throw runtime_error("TCPTransport::makeEndpoint called with invalid address " + address);

	ip::address ipaddr = ip::address::from_string(match[1]);
	unsigned short port = boost::lexical_cast<unsigned short>(match[2]);

	return new TCPEndpoint(ip::tcp::endpoint(ipaddr, port), iothread);
}

