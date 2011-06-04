#include "HAL/SubHAL.h"
#include "HAL/format/Sub7EPacketFormatter.h"
#include "MotorDriverDataObjectFormatter.h"
#include "HeartBeat.h"
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

