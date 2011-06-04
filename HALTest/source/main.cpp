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

static void receiveCallback(int endnum, std::auto_ptr<DataObject> &dobj) {
	cout << "Received a data object on endpoint " << endnum << endl;
}

static void errorCallback(int endnum, const std::string &msg) {
	cout << "Got an error on endpoint " << endnum << ": " << msg << endl;
}

int main(int argc, char **argv) {
	SubHAL hal;
	hal.loadAddressFile("addresses");
	boost::scoped_ptr<DataObjectEndpoint> endpoint(hal.openDataObjectEndpoint(1, new MotorDriverDataObjectFormatter(1), new Sub7EPacketFormatter()));

	while (true) {
		this_thread::sleep(posix_time::seconds(1));

		endpoint->write(HeartBeat());
		cout << "Sent heartbeat" << endl;
	}
}

