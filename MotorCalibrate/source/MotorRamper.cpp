#include "MotorCalibrate/MotorRamper.h"
#include "DataObjects/MotorDriver/SetReference.h"
#include <boost/bind.hpp>

using namespace subjugator;
using namespace boost;
using namespace boost::asio;
using namespace boost::posix_time;
using namespace std;

MotorRamper::MotorRamper(boost::asio::io_service &ioservice, DataObjectEndpoint &endpoint)
: timer(ioservice), endpoint(endpoint) { }

void MotorRamper::configureCallbacks(const RampUpdateCallback &rampupdatecallback, const RampCompleteCallback &rampcompletecallback) {
	this->rampupdatecallback = rampupdatecallback;
	this->rampcompletecallback = rampcompletecallback;
}

void MotorRamper::start(const Settings &settings) {
	this->settings = settings;
	rampdown = false;
	counter = 0;
	curdivision = 0;
	direction = Forward;
	startTimer();
}

void MotorRamper::stop() {
	timer.cancel();
}

void MotorRamper::startTimer() {
	timer.expires_from_now(milliseconds(1000/updateRateHz));
	timer.async_wait(bind(&MotorRamper::timerCallback, this, _1));
}

void MotorRamper::timerCallback(boost::system::error_code &error) {
	if (error)
		return;

	bool done=false;
	double outref;
	if (rampdown) {
		outref = settings.maxreference * (1 - counter / (4*updateRateHz));
		if (outref <= 0) {
			rampdown = false;
			curdivision = 0;
			counter = 0;
			if (direction == Forward)
				direction = Reverse;
			else {
				direction = Forward;
				rampcompletecallback();
				if (!settings.repeat)
					done = true;
			}
		}

		counter++;
	} else {
		int rampcounts = (int)ceil(settings.ramptime * updateRateHz);
		int holdcounts = (int)ceil(settings.holdtime * updateRateHz);

		double rampstartref = settings.maxreference / settings.divisions * curdivision;
		double rampstopref = settings.maxreference / settings.divisions * (curdivision+1);

		if (counter < rampcounts) {
			outref = rampstartref + (rampstopref - rampstartref) * ((double)counter / rampcounts);
		} else {
			outref = rampstopref;
		}

		counter++;
		if (counter == rampcounts + holdcounts) {
			counter = 0;
			curdivision++;

			if (curdivision == settings.divisions)
				rampdown = true;
		}
	}

	if (direction == Reverse)
		outref = -outref;

	endpoint.write(SetReference(outref));
	rampupdatecallback(outref);
	if (!done)
		startTimer();
}

