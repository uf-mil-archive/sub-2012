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

int main(int argc, char **argv) {
	SubHAL hal;
	hal.loadAddressFile("addresses");

	scoped_ptr<DataObjectEndpoint> endpoint(hal.openDataObjectEndpoint(2, new MotorDriverDataObjectFormatter(2, 1, BRUSHEDOPEN), new Sub7EPacketFormatter()));
	endpoint->open();
	hal.startIOThread();

	double reference;
	do {
		cout << "Reference: ";
		cin >> reference;
	} while (reference > 1 || reference < -1);

	endpoint->write(HeartBeat());
	endpoint->write(SetReference(reference));
	while (true) {
		endpoint->write(HeartBeat());
		this_thread::sleep(posix_time::milliseconds(1000));
		cout << "Heartbeat" << endl;
	}
}

