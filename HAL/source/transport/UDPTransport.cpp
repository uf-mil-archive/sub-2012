#include "HAL/transport/UDPTransport.h"
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <algorithm>
#include <memory>
#include <cassert>

using namespace subjugator;
using namespace boost;
using namespace boost::asio;
using namespace boost::system;
using namespace std;

UDPTransport::UDPTransport(io_service &ioservice, int port)
: port(port), socket(ioservice), recvbuffer(4096) { }

const string &UDPTransport::getName() const {
	static const string name = "udp";
	return name;
}

Endpoint *UDPTransport::makeEndpoint(const std::string &address, const ParamMap &params) {
	static const regex ipreg("(\\d+\\.\\d+\\.\\d+\\.\\d+):(\\d+)");
	smatch match;
	if (!regex_match(address, match, ipreg))
		throw runtime_error("UDPTransport::makeEndpoint called with invalid address " + address);

	string ipaddr = match[1];
	unsigned short port = boost::lexical_cast<unsigned short>(match[2]);
	ip::udp::endpoint endpoint(ip::address::from_string(ipaddr), port);

	auto_ptr<UDPEndpoint> udpendpoint(new UDPEndpoint(endpoint, *this));
	endpoints.push_back(udpendpoint.get());
	return udpendpoint.release();
}

void UDPTransport::endpointOpened(UDPEndpoint *endpoint) {
	if (socket.is_open())
		return;

	error_code error;
	socket.open(ip::udp::v4(), error); // open the UDP socket

	if (error) {
		setError("UDPTransport failed to open socket: " + error.message());
		return;
	}

	socket.bind(ip::udp::endpoint(ip::udp::v4(), port), error);

	if (error) {
		setError("UDPTransport failed to bind socket: " + error.message());
		return;
	}

	socket.set_option(socket_base::broadcast(true), error);

	if (error) {
		setError("UDPTransport failed to enable broadcast on socket: " + error.message());
		return;
	}

	setError("");
	startAsyncReceive();
	if (sendqueue.size() > 0)
		startAsyncSend();
}

void UDPTransport::endpointWrite(UDPEndpoint *endpoint, ByteVec::const_iterator begin, ByteVec::const_iterator end) {
	// we can't touch the send queue, io thread has exclusive use of it
	// so, we run a callback on the io thread which will do the work for us
	socket.get_io_service().dispatch(bind(&UDPTransport::pushSendQueueCallback, this, endpoint->getEndpoint(), ByteVec(begin, end)));
}

void UDPTransport::endpointClosed(UDPEndpoint *endpoint) { }

void UDPTransport::endpointDeleted(UDPEndpoint *endpoint) {
	for (EndpointPtrVec::iterator i = endpoints.begin(); i != endpoints.end(); ++i) {
		if (*i == endpoint) {
			endpoints.erase(i);
			return;
		}
	}
}

const string &UDPTransport::getEndpointError() const { return errmsg; }

void UDPTransport::pushSendQueueCallback(const ip::udp::endpoint &endpoint, const ByteVec &bytes) {
    // we're in the io thread now, so we can manipulate the send queue
	bool sendpending = !sendqueue.empty(); // if the queue isn't empty, a send must be pending for the one on top
	sendqueue.push(make_pair(endpoint, bytes)); // put our new packet at the end

	if (!sendpending) // if there was no send pending previously
		startAsyncSend(); // start one now
}

void UDPTransport::sendCallback(const system::error_code& error, std::size_t bytes) {
	if (error) {
		setError("Error while sending: " + error.message());
		return;
	}

	sendqueue.pop(); // pop the now sent packet off the queue
	if (!sendqueue.empty()) // if there is another packet waiting
		startAsyncSend(); // start another send
}

void UDPTransport::receiveCallback(const system::error_code& error, std::size_t bytes) {
	if (error) {
		setError("Error while receiving: " + error.message());
		return;
	}

	UDPEndpoint *endpoint = NULL; // first, determine the endpoint we received from
	for (EndpointPtrVec::iterator i = endpoints.begin(); i != endpoints.end(); ++i) { // loop through all UDPEndpoint instances
		if ((*i)->getEndpoint() == recvendpoint) { // if we find one with an endpoint matching the one we just got on a packet
			if ((*i)->getState() == UDPEndpoint::OPEN) // check if the UDPEndpoint is open
				endpoint = *i; // if it is, we've got a match
			break;
		}
	}

	if (endpoint) // if we got a match
		endpoint->packetReceived(recvbuffer.begin(), recvbuffer.begin() + bytes);	// inform the UDPEndpoint it just received a packet
	startAsyncReceive(); // start another async receive
}

void UDPTransport::startAsyncSend() {
	assert(!sendqueue.empty());

	const ip::udp::endpoint &endpoint = sendqueue.front().first;
	const ByteVec &data = sendqueue.front().second;

	socket.async_send_to(buffer(data), endpoint, bind(&UDPTransport::sendCallback, this, _1, _2));
}

void UDPTransport::startAsyncReceive() {
	socket.async_receive_from(buffer(recvbuffer), recvendpoint, bind(&UDPTransport::receiveCallback, this, _1, _2));
}

void UDPTransport::setError(const std::string &errmsg) {
	if (this->errmsg == errmsg)
		return;

	this->errmsg = errmsg;
	for (EndpointPtrVec::iterator i = endpoints.begin(); i != endpoints.end(); ++i) {
		(*i)->errorChanged();
	}

	if (errmsg.size())
		socket.close();
}

