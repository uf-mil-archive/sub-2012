#include "MotorCalibrate/MotorBangBang.h"
#include "DataObjects/MotorDriver/SetReference.h"
#include <boost/bind.hpp>
#include <cstdlib>

using namespace subjugator;
using namespace boost;
using namespace boost::asio;
using namespace boost::posix_time;
using namespace std;

MotorBangBang::MotorBangBang(boost::asio::io_service &ioservice, DataObjectEndpoint &endpoint)
: timer(ioservice), endpoint(endpoint) { }

void MotorBangBang::configureCallbacks(const BangUpdateCallback &bangupdatecallback) {
	this->bangupdatecallback = bangupdatecallback;
}

void MotorBangBang::start(const Settings &settings) {
	this->settings = settings;
	toggle = false;
	startTimer();
}

void MotorBangBang::stop() {
	timer.cancel();
}

void MotorBangBang::startTimer() {
	timer.expires_from_now(milliseconds(1000*settings.holdtime));
	timer.async_wait(bind(&MotorBangBang::timerCallback, this, _1));
}

void MotorBangBang::timerCallback(boost::system::error_code &error) {
	if (error)
		return;

	double outref;
	if (settings.random) {
		outref = (rand() % 100) / 100.0 * settings.maxreference;
		if (rand() % 2)
			outref = -outref;
	} else {
		toggle = !toggle;
		if (toggle)
			outref = -settings.maxreference;
		else
			outref = settings.maxreference;
	}

	endpoint.write(SetReference(outref));
	bangupdatecallback(outref);
	startTimer();
}

