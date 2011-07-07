#include "SubMain/Workers/PDWorker/MergeManager.h"
#include "DataObjects/Merge/MergeDataObjectFormatter.h"
#include "DataObjects/Embedded/StartPublishing.h"
#include "DataObjects/HeartBeat.h"
#include "DataObjects/Merge/MergeActuator.h"
#include "HAL/format/Sub7EPacketFormatter.h"
#include <boost/bind.hpp>

using namespace subjugator;
using namespace boost;
using namespace std;

#include <iostream>

MergeManager::MergeManager(SubHAL &hal)
: endpoint(hal.openDataObjectEndpoint(60, new MergeDataObjectFormatter(60, 21), new Sub7EPacketFormatter())) {
	endpoint->configureCallbacks(bind(&MergeManager::halReceiveCallback, this, _1), bind(&MergeManager::halStateChangeCallback, this));
	endpoint->open();
}

void MergeManager::halReceiveCallback(std::auto_ptr<DataObject> &dobj) {
	if (MergeInfo *info = dynamic_cast<MergeInfo *>(dobj.get())) {
		cout << "Got merge info" << endl;
		this->info = *info;
	}
}

void MergeManager::halStateChangeCallback() {
	if (endpoint->getState() == Endpoint::OPEN) {
		endpoint->write(HeartBeat());
		endpoint->write(StartPublishing(10));
	}
}

void MergeManager::setActuators(int flags) {
	endpoint->write(MergeActuator(flags));
}

