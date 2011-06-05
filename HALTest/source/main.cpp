#include "HAL/SubHAL.h"
#include "HAL/format/Sub7EPacketFormatter.h"
#include "DataObjects/MotorDriver/MotorDriverDataObjectFormatter.h"
#include "DataObjects/MotorDriver/StartPublishing.h"
#include "DataObjects/MotorDriver/StopPublishing.h"
#include "DataObjects/MotorDriver/SetReference.h"
#include "DataObjects/HeartBeat.h"
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>
#include <vector>
#include <string>
#include <iostream>

using namespace subjugator;
using namespace boost;
using namespace std;

static void rampUp(double start, double stop, double inc, double sleep, scoped_ptr<DataObjectEndpoint> &endpoint) {
	double reference=start;

	while (reference <= stop) {
		endpoint->write(HeartBeat());
		endpoint->write(SetReference(reference));
		this_thread::sleep(posix_time::milliseconds(sleep*1000));
		cout << "Ref " << reference << endl;
		reference += inc;
	}

	endpoint->write(SetReference(stop));
}

static void rampDown(double start, double stop, double inc, double sleep, scoped_ptr<DataObjectEndpoint> &endpoint) {
	double reference=start;

	while (reference >= stop) {
		endpoint->write(HeartBeat());
		endpoint->write(SetReference(reference));
		this_thread::sleep(posix_time::milliseconds(sleep*1000));
		cout << "Ref " << reference << endl;
		reference -= inc;
	}

	endpoint->write(SetReference(stop));
}

static void wait(double sleep, scoped_ptr<DataObjectEndpoint> &endpoint) {
	int counts = (int)(sleep / .1);
	for (int ctr=0; ctr<counts; ctr++) {
		endpoint->write(HeartBeat());
		this_thread::sleep(posix_time::milliseconds(100));
	}
}

int main(int argc, char **argv) {
	SubHAL hal;
	scoped_ptr<DataObjectEndpoint> endpoint(hal.openDataObjectEndpoint(2, new MotorDriverDataObjectFormatter(2, 1, BRUSHEDOPEN), new Sub7EPacketFormatter()));
	endpoint->open();
	hal.startIOThread();

	if (endpoint->getState() == Endpoint::ERROR) {
		cout << "Endpoint entered error state: " << endpoint->getErrorMessage() << endl;
		return 1;
	}

	rampUp(0, 1, .01, .1, endpoint);
	wait(2, endpoint);
	rampDown(1, 0, .01, .1, endpoint);
	wait(1, endpoint);
	rampDown(0, -1, .01, .1, endpoint);
	wait(2, endpoint);
	rampUp(-1, 0, .01, .1, endpoint);
	wait(1, endpoint);

	endpoint->close();
}

