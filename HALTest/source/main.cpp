#include "HAL/HAL.h"
#include "HAL/Sub7EPacketFormatter.h"
#include "HAL/SerialTransport.h"
#include "MotorDriverDataObjectFormatter.h"
#include "HeartBeat.h"
#include <boost/assign.hpp>
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
	vector<string> ports;
	ports.push_back("/dev/ttyUSB0");

	HAL hal(new MotorDriverDataObjectFormatter(), new SerialTransport(ports), Sub7EPacketFormatter::factory);
	hal.configureCallbacks(receiveCallback, errorCallback);
	hal.start();

	while (true) {
		this_thread::sleep(posix_time::seconds(1));

		HeartBeat hb(1);
		hal.write(0, hb);
		cout << "Sent heartbeat" << endl;
	}
}

