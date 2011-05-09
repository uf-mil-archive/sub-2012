#include "HAL/UDPTransport.h"
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <algorithm>
#include <cassert>

using namespace subjugator;
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace std;

UDPTransport::UDPTransport(const vector<EndpointConfig> &endpointconfigs)
: socket(ioservice, udp::v4()),
  endpoints(endpointconfigs.size()),
  recvbuffer(4096) {
	for (int i=0; i < endpointconfigs.size(); i++) {
		const EndpointConfig &config = endpointconfigs[i];
		endpoints[i] = udp::endpoint(address::from_string(config.first), config.second); // create asio endpoint objects for each end point config
	}

	startAsyncReceive(); // start the first async receive
}

UDPTransport::~UDPTransport() {
	stopIOThread();
}

int UDPTransport::getEndpointCount() const {
	return endpoints.size();
}

void UDPTransport::write(int endnum, const ByteVec &bytes) {
	// we can't touch the send queue, io thread has exclusive use of it
	// so, we run a callback on the io thread which will do the work for us
	ioservice.dispatch(bind(&UDPTransport::asioPushSendQueueCallback, this, endnum, bytes));
}

void UDPTransport::asioPushSendQueueCallback(int endnum, const ByteVec &bytes) {
    // we're in the io thread now, so we can manipulate the send queue
	bool sendpending = !sendqueue.empty(); // if the queue isn't empty, a send must be pending for the one on top
	sendqueue.push(make_pair(endnum, bytes)); // put our new packet at the end

	if (!sendpending) // if there was no send pending previously
		startAsyncSend(); // start one now
}

void UDPTransport::asioSendCallback(const boost::system::error_code& error, std::size_t bytes) {
	if (error) {
		if (errorcallback)
			errorcallback(sendqueue.front().first, "UDPTransport received error while sending: " + lexical_cast<string>(error)); // call the error callback
	}

	sendqueue.pop(); // pop the now sent packet off the queue
	if (!sendqueue.empty()) // if there is another packet waiting
		startAsyncSend(); // start another send
}

void UDPTransport::asioReceiveCallback(const boost::system::error_code& error, std::size_t bytes) {
	int endnum; // first, determine the endpoint number we received from
	for (endnum=0; endnum<endpoints.size(); endnum++) { // loop through all endpoints
		if (recvendpoint == endpoints[endnum]) // if we get a match, stop
			break;
	}

	if (endnum == endpoints.size()) // no match?
		return; // drop the packet

	if (!error) { // successful read
		recvbuffer.resize(bytes); // resize the buffer to the number of bytes asio put in there (asio never resizes vectors)
		if (readcallback)
			readcallback(endnum, recvbuffer); // call the read callback
		recvbuffer.resize(recvbuffer.capacity()); // resize the buffer back to its maximum capacity
	} else { // error did occur
		if (errorcallback)
			errorcallback(endnum, "UDPTransport received error while receiving: " + lexical_cast<string>(error)); // call the error callback
	}

	// finally, initiate another asynchronous receive
	startAsyncReceive();
}

void UDPTransport::startAsyncSend() {
	assert(!sendqueue.empty());

	int endnum = sendqueue.front().first;
	const ByteVec &data = sendqueue.front().second;
	socket.async_send_to(buffer(data), endpoints[endnum], bind(&UDPTransport::asioSendCallback, this, _1, _2));
}

void UDPTransport::startAsyncReceive() {
	socket.async_receive_from(buffer(recvbuffer), recvendpoint, bind(&UDPTransport::asioReceiveCallback, this, _1, _2));
}

