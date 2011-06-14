#include "HAL/SubHAL.h"
#include "DataObjects/DVL/DVLDataObjectFormatter.h"
#include "DataObjects/DVL/DVLPacketFormatter.h"
#include "DataObjects/DVL/DVLConfiguration.h"
#include "DataObjects/DVL/DVLBreak.h"
#include "DataObjects/DVL/DVLHighresBottomTrack.h"
#include "DataObjects/DVL/DVLBottomTrackRange.h"
#include "DataObjects/DVL/DVLBottomTrack.h"
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>
#include <iostream>

using namespace subjugator;
using namespace boost;
using namespace boost::posix_time;
using namespace std;

void stateChangeCallback() {

}

void receiveCallback(auto_ptr<DataObject> &dobj) {
	if (DVLHighresBottomTrack *hrtrack = dynamic_cast<DVLHighresBottomTrack *>(dobj.get())) {
		cout << "Got bottom track: ";
		if (hrtrack->isGood())
			cout << hrtrack->getVelocity()(0) << " " << hrtrack->getVelocity()(1) << " " << hrtrack->getVelocity()(2) << endl;
		else
			cout << "bad" << endl;
	} else if (DVLBottomTrackRange *btrange = dynamic_cast<DVLBottomTrackRange *>(dobj.get())) {
		cout << "Got bottom range: ";
		if (btrange->isGood())
			cout << btrange->getRange() << endl;
		else
			cout << "bad" << endl;
	} else if (DVLBottomTrack *bt = dynamic_cast<DVLBottomTrack *>(dobj.get())) {
		cout << "Got bottom track correlation: ";
		cout << bt->getBeamCorrelation()(0) << " " << bt->getBeamCorrelation()(1) << " " << bt->getBeamCorrelation()(2) << " " << bt->getBeamCorrelation()(3) << " " << endl;
	}
}

int main(int argc, char **argv) {
	SubHAL hal;
	scoped_ptr<DataObjectEndpoint> endpoint(hal.openDataObjectEndpoint(50, new DVLDataObjectFormatter(), new DVLPacketFormatter()));

	endpoint->configureCallbacks(receiveCallback, stateChangeCallback);
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

