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
using namespace std;

SerialTransport::SerialTransport(const vector<string> &devicenames)
: endpointdatavec(devicenames.size()) {
	for (int endnum=0; endnum<devicenames.size(); endnum++) {
		endpointdatavec.push_back(new EndpointData(ioservice));
		EndpointData &edata = endpointdatavec.back();

		edata.port.open(devicenames[endnum]);
		startAsyncReceive(endnum);
	}

	startIOThread();
}

SerialTransport::~SerialTransport() {
	stopIOThread();
}

int SerialTransport::getEndpointCount() const {
	return endpointdatavec.size();
}

void SerialTransport::write(int endnum, const ByteVec &bytes) {
	// io thread owns all of the endpointdata, we can't touch it here
	// so we run a callback in the io thread to do our work
	ioservice.dispatch(bind(&SerialTransport::asioAppendSendBufCallback, this, endnum, bytes));
}

void SerialTransport::asioAppendSendBufCallback(int endnum, const ByteVec &bytes) {
    // we're in the io thread now, so we can manipulate endpointdata
    EndpointData &edata = endpointdatavec[endnum];
    bool sendpending = !edata.sendbuf.empty(); // if the send buffer isn't empty, a send must be pending
    edata.sendbuf.insert(edata.sendbuf.end(), bytes.begin(), bytes.end()); // append new data to the end of the send buffer

    if (!sendpending) // if there was no send pending
    	startAsyncSend(endnum); // start one now
}

void SerialTransport::asioSendCallback(int endnum, const boost::system::error_code& error, std::size_t bytes) {
	EndpointData &edata = endpointdatavec[endnum];

	if (error) {
		if (errorcallback)
			errorcallback(endnum, "SerialTransport received error while sending: " + lexical_cast<string>(error)); // call the error callback
		// TODO close socket?
		return;
	}

	edata.sendbuf.erase(edata.sendbuf.begin(), edata.sendbuf.begin() + bytes); // erase the bytes from the send buffer
	if (!edata.sendbuf.empty()) // if there is still data in the send buffer
		startAsyncSend(endnum); // start another send
}

void SerialTransport::asioReceiveCallback(int endnum, const boost::system::error_code& error, std::size_t bytes) {
	EndpointData &edata = endpointdatavec[endnum];

	if (error) {
		if (errorcallback)
			errorcallback(endnum, "SerialTransport received error while receiving: " + lexical_cast<string>(error)); // call the error callback
		// TODO close socket?
		return;
	}

	edata.recvbuf.resize(bytes); // resize the buffer to the number of bytes asio put in there (asio never resizes vectors)
	if (readcallback)
		readcallback(endnum, edata.recvbuf); // call the read callback
	edata.recvbuf.resize(edata.recvbuf.capacity()); // resize the buffer back to its maximum capacity

	// finally, initiate another asynchronous receive
	startAsyncReceive(endnum);
}

void SerialTransport::startAsyncSend(int endnum) {
	EndpointData &edata = endpointdatavec[endnum];
	assert(!edata.sendbuf.empty());

	edata.port.async_write_some(buffer(edata.sendbuf), bind(&SerialTransport::asioSendCallback, this, endnum, _1, _2));
}

void SerialTransport::startAsyncReceive(int endnum) {
	EndpointData &edata = endpointdatavec[endnum];
	edata.port.async_read_some(buffer(edata.recvbuf), bind(&SerialTransport::asioReceiveCallback, this, endnum, _1, _2));
}

