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
: StreamTransport<boost::asio::ip::tcp::socket>(endpointconfigs.size()) {
	for (int endnum=0; endnum<endpointconfigs.size(); endnum++) { // for every end point config
		streamdatavec.push_back(new StreamData(ioservice)); // allocate stream data
		StreamData &sdata = streamdatavec.back();

		const EndpointConfig &econfig = endpointconfigs[endnum]; // create asio endpoint objects for each end point config
		tcp::endpoint endpoint(address::from_string(econfig.first), econfig.second);

		sdata.stream.async_connect(endpoint, bind(&TCPTransport::asioConnectCallback, this, endnum, _1)); // start an async connect
	}
}

TCPTransport::~TCPTransport() {
	stopIOThread();
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

