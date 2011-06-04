#include "HAL/SubHAL.h"
#include "HAL/format/Sub7EPacketFormatter.h"
#include "DataObjects/MotorDriver/MotorDriverDataObjectFormatter.h"
#include "DataObjects/MotorDriver/HeartBeat.h"
#include "DataObjects/MotorDriver/StartPublishing.h"
#include "DataObjects/MotorDriver/SetNewReference.h"
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>
#include <vector>
#include <string>
#include <iostream>

using namespace subjugator;
using namespace boost;
using namespace std;

static void receiveCallback(std::auto_ptr<DataObject> &dobj) {
	cout << "Received a data object object callback" << endl;
}

static void stateChangeCallback() {
	cout << "Got a state change callback" << endl;
}

int main(int argc, char **argv) {
	SubHAL hal;
	hal.loadAddressFile("addresses");
	boost::scoped_ptr<DataObjectEndpoint> endpoint(hal.openDataObjectEndpoint(1, new MotorDriverDataObjectFormatter(1), new Sub7EPacketFormatter()));

	endpoint->configureCallbacks(receiveCallback, stateChangeCallback);
	endpoint->open();

	hal.startIOThread();
	this_thread::sleep(posix_time::seconds(.1));

	double reference;
	do {
		cout << "Reference: ";
		cin >> reference;
	} while (reference > 1 || reference < -1);

	endpoint->write(HeartBeat());
	endpoint->write(SetNewReference(reference));

	while (true) {
		if (endpoint->getState() == Endpoint::ERROR) {
			cout << "Endpoint entered error state: " << endpoint->getErrorMessage() << endl;
			break;
		}

		endpoint->write(HeartBeat());
		cout << "Sent heartbeat" << endl;

		this_thread::sleep(posix_time::seconds(1));
	}
}

