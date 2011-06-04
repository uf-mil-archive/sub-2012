#include "HAL/TCPEndpoint.h"
#include <cassert>

using namespace subjugator;
using namespace boost;
using namespace boost::asio;
using namespace std;

TCPEndpoint::TCPEndpoint(const ip::tcp::endpoint &endpoint, IOThread &iothread)
: BaseStreamEndpoint<ip::tcp::socket>(iothread), endpoint(endpoint) { }

void TCPEndpoint::open() {
	assert(getState() == CLOSED);
	stream.async_connect(endpoint, bind(&TCPEndpoint::connectCallback, this, _1));
}

void TCPEndpoint::connectCallback(const boost::system::error_code& error) {
	if (!error) { // if no error occured
		setState(OPEN); // we're now open
		startAsyncSendReceive(); // start sending and receiving
	} else {
		setState(ERROR, "TCPTransport received error while connecting: " + lexical_cast<string>(error)); // otherwise, record the error
	}
}

