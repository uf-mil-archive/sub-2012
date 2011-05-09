#include "HAL/SerialTransport.h"
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/scoped_ptr.hpp>
#include <algorithm>
#include <cassert>

using namespace subjugator;
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace boost::system;
using namespace std;

SerialTransport::SerialTransport(const vector<string> &devicenames)
: StreamTransport<boost::asio::serial_port>(devicenames.size()), devicenames(devicenames) { }

SerialTransport::~SerialTransport() {
	stop();
}

void SerialTransport::start() {
	error_code error;

	for (int endnum=0; endnum<streamdatavec.size(); endnum++) { // for each endpoint
		StreamData &sdata = streamdatavec[endnum];
		sdata.stream.open(devicenames[endnum], error); // open a serial port

		if (!error) {
			startAsyncReceive(endnum); // and start an async receive
		} else { // if it failed to open
			if (errorcallback) {
				string msg = "SerialTransport failed to open serial device " + devicenames[endnum] + ": " + lexical_cast<string>(error);
				runCallbackOnIOThread(bind(errorcallback, endnum, msg)); // invoke the error callback on the IO thread
			}
		}
	}

	startIOThread();
}

void SerialTransport::stop() {
	stopIOThread();

	for (int endnum=0; endnum<streamdatavec.size(); endnum++) {
		StreamData &sdata = streamdatavec[endnum];
		sdata.stream.close();
	}
}

