#include "MotorCalibrate/HeartBeatSender.h"
#include "DataObjects/HeartBeat.h"
#include <boost/bind.hpp>

using namespace subjugator;
using namespace boost::asio;
using namespace boost::posix_time;
using namespace boost::system;
using namespace boost;
using namespace std;

HeartBeatSender::HeartBeatSender(io_service &io_service, DataObjectEndpoint &endpoint, double hz)
: timer(io_service), endpoint(endpoint), hz(hz) { }

void HeartBeatSender::start() {
	timer.expires_from_now(milliseconds(1000/hz));
	timer.async_wait(bind(&HeartBeatSender::timerCallback, this, _1));
}

void HeartBeatSender::stop() {
	timer.cancel();
}

void HeartBeatSender::timerCallback(boost::system::error_code &error) {
	if (error)
		return;

	endpoint.write(HeartBeat());
	start();
}

