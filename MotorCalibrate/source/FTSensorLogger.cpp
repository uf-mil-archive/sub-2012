#include "MotorCalibrate/FTSensorLogger.h"
#include <stdexcept>
#include <boost/regex.hpp>
#include <boost/thread.hpp>

enum {
	ACK = 6,
	NACK = 21
};

using namespace subjugator;
using namespace boost;
using namespace boost::asio;
using namespace boost::posix_time;
using namespace std;

FTSensorLogger::FTSensorLogger(const std::string &device, io_service &ioservice, const LogCallback &logcallback)
: port(ioservice), device(device), running(false), recvstream(&recvbuf), logcallback(logcallback) {
}

void FTSensorLogger::bias() {
	assert(!running);
	sendCommandACK("SB");
}

void FTSensorLogger::begin() {
	assert(!running);

	if (!port.is_open()) {
		port.open(device);
		port.set_option(serial_port::baud_rate(9600));
		port.set_option(serial_port::flow_control(serial_port::flow_control::none));

		sync();
	}

	sendCommandACK("CD A");
	sendCommandACK("CD R");

	sendCommand("QS");
	int got = read_until(port, recvbuf, ACK);
	recvbuf.consume(got);

	async_read_until(port, recvbuf, '\n', boost::bind(&FTSensorLogger::receiveCallback, this, _1, _2));
	running = true;
}

void FTSensorLogger::end() {
	assert(running);
	port.cancel();
	running = false;
	sync();
}

void FTSensorLogger::sendCommand(const std::string &command) {
	string commandstr = command + "\r";
	write(port, buffer(commandstr), transfer_all());
}

void FTSensorLogger::sendCommandACK(const std::string &command) {
	sendCommand(command);

	read_until(port, recvbuf, ">");

	string line;
	getlineCRLN(recvstream, line);
	if (line != command)
		throw runtime_error("Command '" + command + "' wasn't echod properly. Got: '" + line + "'");

	getlineCRLN(recvstream, line);
	if (line[0] != ACK || line[1] != ACK)
		throw runtime_error("Command '" + command + "' wasn't ACKd properly");

	recvbuf.consume(recvbuf.size());
}

void FTSensorLogger::receiveCallback(const boost::system::error_code& error, std::size_t bytes) {
	if (error) {
		if (error == error::operation_aborted)
			return;
		throw runtime_error("FTSensorLogger got error in receiveCallback: " + error.message());
	}

	int drop;
	LogEntry entry;
	recvstream >> drop; recvstream.get(); // some initial value
	recvstream >> entry.fx; recvstream.get(); // skip the comma
	recvstream >> entry.fy; recvstream.get();
	recvstream >> entry.fz; recvstream.get();
	recvstream >> entry.mx; recvstream.get();
	recvstream >> entry.my; recvstream.get();
	recvstream >> entry.mz;
	logcallback(entry);

	string line;
	getline(recvstream, line);

	async_read_until(port, recvbuf, "\r\n", boost::bind(&FTSensorLogger::receiveCallback, this, _1, _2));
}

void FTSensorLogger::getlineCRLN(istream &in, string &line) {
	getline(in, line);
	if (line[line.size()-1] == '\r')
		line.resize(line.size()-1);
}

void FTSensorLogger::sync() {
	sendCommand("");
	this_thread::sleep(milliseconds(500));
	clearReadBuffer();
	recvbuf.consume(recvbuf.size());
}

// unfortunately ASIO doesn't seem to have the ability to clear the read buffer of a serial port
// but this really is needed in the FTSensorLogger's interactive protocol. Otherwise
// we have no way of telling if we're reading old data left over from a previous run of the program
// or new data sent in reply to the command we just sent.
// I might actually try and get this into boost sometime soon, there's more than a few people on the internet
// complaining about its non-existance.

#ifdef __WIN32__
void FTSensorLogger::clearReadBuffer(boost::asio::serial_port &port) { } // don't have a windows implementation ATM, but apparently isn't needed
#else

// break out some posix APIs
#include <termios.h>
#include <errno.h>

void FTSensorLogger::clearReadBuffer() {
	int portfd = port.native();
	if (tcflush(portfd, TCIFLUSH) == -1)
		throw runtime_error("FTSensorLogger got error while clearing read buffer: " + string(strerror(errno)));
}

#endif

