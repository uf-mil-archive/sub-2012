#include "MotorCalibrate/FTSensorLogger.h"
#include <HAL/IOThread.h>
#include <boost/thread.hpp>
#include <iostream>

using namespace subjugator;
using namespace boost;
using namespace std;

static void logCallback(const string &log) {
	cout << "Got log callback: " << log << endl;
}

int main(int argc, char **argv) {
	IOThread iothread;

	FTSensorLogger logger("/dev/ttyUSB0", iothread.getIOService(), logCallback);
	logger.begin();

	iothread.start();

	while (true)
		this_thread::sleep(posix_time::seconds(.1));
}

