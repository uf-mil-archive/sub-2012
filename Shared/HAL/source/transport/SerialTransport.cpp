#include "HAL/transport/SerialTransport.h"
#include "HAL/transport/SerialEndpoint.h"
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/scoped_ptr.hpp>
#include <algorithm>
#include <cassert>

using namespace subjugator;
using namespace boost;
using namespace boost::asio;
using namespace std;

SerialTransport::SerialTransport(io_service &ioservice) : ioservice(ioservice) { }

const string &SerialTransport::getName() const {
	static const string name = "serial";
	return name;
}

Endpoint *SerialTransport::makeEndpoint(const std::string &address, const ParamMap &parammap) {
	int baud;
	ParamMap::const_iterator i = parammap.find("baud");
	if (i != parammap.end())
		baud = lexical_cast<int>(i->second);
	else
		baud = 115200;

	return new SerialEndpoint(address, baud, ioservice);
}

