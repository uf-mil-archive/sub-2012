#include "HAL/transport/SerialEndpoint.h"
#include <cassert>

using namespace subjugator;
using namespace boost;
using namespace boost::system;
using namespace boost::asio;
using namespace std;

SerialEndpoint::SerialEndpoint(const std::string &devicename, int baud, boost::asio::io_service &io) :
BaseStreamEndpoint<serial_port>(io), devicename(devicename), baud(baud) { }

void SerialEndpoint::open() {
	assert(getState() == CLOSED);

	error_code error;
	stream.open(devicename, error);

	if (error) {
		setState(ERROR, "SerialEndpoint failed to open serial port " + devicename + ": " + error.message());
		return;
	}

	stream.set_option(serial_port::baud_rate(baud), error);
	if (error) {
		setState(ERROR, "SerialEndpoint failed to set baud rate on serial port " + devicename + " to " + lexical_cast<string>(baud) + ": " + error.message());
		return;
	}

	stream.set_option(serial_port::flow_control(serial_port::flow_control::none), error);
	if (error) {
		setState(ERROR, "SerialEndpoint failed to disable flow control on serial port " + devicename + ": " + error.message());
		return;
	}

	setState(OPEN);
	startAsyncSendReceive();
}

