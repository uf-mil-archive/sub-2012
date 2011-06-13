#include "HAL/SubHAL.h"
#include "DataObjects/DVL/DVLDataObjectFormatter.h"
#include "DataObjects/DVL/DVLPacketFormatter.h"
#include "DataObjects/DVL/DVLConfiguration.h"
#include "DataObjects/DVL/DVLBreak.h"
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>
#include <iostream>

using namespace subjugator;
using namespace boost;
using namespace boost::posix_time;
using namespace std;

int main(int argc, char **argv) {
	SubHAL hal;
	scoped_ptr<DataObjectEndpoint> endpoint(hal.openDataObjectEndpoint(50, new DVLDataObjectFormatter(), new DVLPacketFormatter()));

	endpoint->open();
	hal.startIOThread();

	cout << "Break" << endl;
	endpoint->write(DVLBreak());
	this_thread::sleep(seconds(2));

	cout << "Go" << endl;
	endpoint->write(DVLConfiguration(15, 0));

	while (true)
		this_thread::sleep(seconds(1));
}

