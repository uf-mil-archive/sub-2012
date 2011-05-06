#include "HAL/TCPTransport.h"
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

TCPTransport::TCPTransport(const vector<EndpointConfig> &endpointconfigs)
: endpointdatavec(endpointconfigs.size()) {
	for (int endnum=0; endnum<endpointconfigs.size(); endnum++) {
		endpointdatavec.push_back(new EndpointData(ioservice));

		EndpointData &edata = endpointdatavec.back();
		const EndpointConfig &econfig = endpointconfigs[endnum];
		tcp::endpoint endpoint(address::from_string(econfig.first), econfig.second); // create asio endpoint objects for each end point config

		edata.socket.async_connect(endpoint, bind(&TCPTransport::asioConnectCallback, this, endnum, _1)); // start an async connect
	}

	iothread = thread(bind(&io_service::run, &ioservice)); // and start the io_service in its own thread
}

TCPTransport::~TCPTransport() {
	ioservice.stop(); // tell the ioservice to stop its event loop
	iothread.join(); // wait for the iothread to terminate, so that we can safely destroy the objects that it uses
}

int TCPTransport::getEndpointCount() const {
	return endpointdatavec.size();
}

void TCPTransport::configureCallbacks(ReadCallback readcallback, ErrorCallback errorcallback) {
	this->readcallback = readcallback;
	this->errorcallback = errorcallback;
}

void TCPTransport::write(int endnum, const ByteVec &bytes) {
	// io thread owns all of the endpointdatavec, we can't touch it here
	// so we run a callback in the io thread to do our work
	ioservice.dispatch(bind(&TCPTransport::asioAppendSendBufCallback, this, endnum, bytes));
}

void TCPTransport::asioAppendSendBufCallback(int endnum, const ByteVec &bytes) {
    // we're in the io thread now, so we can manipulate endpointdatavec
    EndpointData &edata = endpointdatavec[endnum];
    bool sendpending = !edata.sendbuf.empty(); // if the send buffer isn't empty, a send must be pending
    edata.sendbuf.insert(edata.sendbuf.end(), bytes.begin(), bytes.end()); // append new data to the end of the send buffer

    if (!sendpending) // if there was no send pending
    	startAsyncSend(endnum); // start one now
}

void TCPTransport::asioSendCallback(int endnum, const boost::system::error_code& error, std::size_t bytes) {
	EndpointData &edata = endpointdatavec[endnum];

	if (error) {
		if (errorcallback)
			errorcallback(endnum, "TCPTransport received error while sending: " + lexical_cast<string>(error)); // call the error callback
		// TODO close socket?
		return;
	}

	edata.sendbuf.erase(edata.sendbuf.begin(), edata.sendbuf.begin() + bytes); // erase the bytes from the send buffer
	if (!edata.sendbuf.empty()) // if there is still data in the send buffer
		startAsyncSend(endnum); // start another send
}

void TCPTransport::asioReceiveCallback(int endnum, const boost::system::error_code& error, std::size_t bytes) {
	EndpointData &edata = endpointdatavec[endnum];

	if (error) {
		if (errorcallback)
			errorcallback(endnum, "TCPTransport received error while receiving: " + lexical_cast<string>(error)); // call the error callback
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

void TCPTransport::asioConnectCallback(int endnum, const boost::system::error_code& error) {
	if (!error) { // if no error occured
		startAsyncReceive(endnum); // begin the first asynchronous receive
	} else {
		if (errorcallback)
			errorcallback(endnum, "TCPTransport received error while connecting: " + lexical_cast<string>(error)); // call the error callback
		// TODO close socket?
		return;
	}
}

void TCPTransport::startAsyncSend(int endnum) {
	EndpointData &edata = endpointdatavec[endnum];
	assert(!edata.sendbuf.empty());

	edata.socket.async_write_some(buffer(edata.sendbuf), bind(&TCPTransport::asioSendCallback, this, endnum, _1, _2));
}

void TCPTransport::startAsyncReceive(int endnum) {
	EndpointData &edata = endpointdatavec[endnum];
	edata.socket.async_read_some(buffer(edata.recvbuf), bind(&TCPTransport::asioReceiveCallback, this, endnum, _1, _2));
}

