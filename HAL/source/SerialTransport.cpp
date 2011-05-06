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
: StreamTransport<boost::asio::serial_port>(devicenames.size()) {
	for (int endnum=0; endnum<devicenames.size(); endnum++) {
		streamdatavec.push_back(new StreamData(ioservice));
		StreamData &sdata = streamdatavec.back();

		sdata.stream.open(devicenames[endnum]);
		startAsyncReceive(endnum);
	}

	startIOThread();
}

SerialTransport::~SerialTransport() {
	stopIOThread();
}

