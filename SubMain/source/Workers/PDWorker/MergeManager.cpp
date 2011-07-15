#include "SubMain/Workers/PDWorker/MergeManager.h"
#include "DataObjects/Merge/MergeDataObjectFormatter.h"
#include "DataObjects/Embedded/StartPublishing.h"
#include "DataObjects/HeartBeat.h"
#include "DataObjects/Actuator/SetActuator.h"
#include "DataObjects/Actuator/ActuatorDataObjectFormatter.h"
#include "DataObjects/Actuator/LimitSwitchStatus.h"
#include "HAL/format/Sub7EPacketFormatter.h"
#include "HAL/format/BytePacketFormatter.h"
#include <boost/bind.hpp>
#include <iostream>

using namespace subjugator;
using namespace boost;
using namespace std;

MergeManager::MergeManager(SubHAL &hal)
: endpoint(hal.openDataObjectEndpoint(60, new MergeDataObjectFormatter(60, 21), new Sub7EPacketFormatter())),
  actuatorendpoint(hal.openDataObjectEndpoint(61, new ActuatorDataObjectFormatter(), new BytePacketFormatter())) {
	endpoint->configureCallbacks(bind(&MergeManager::halReceiveCallback, this, _1), bind(&MergeManager::halStateChangeCallback, this));
	endpoint->open();
	actuatorendpoint->configureCallbacks(bind(&MergeManager::halReceiveCallback, this, _1), bind(&MergeManager::halActuatorStateChangeCallback, this));
	actuatorendpoint->open();
}

void MergeManager::halReceiveCallback(std::auto_ptr<DataObject> &dobj) {
	if (MergeInfo *info = dynamic_cast<MergeInfo *>(dobj.get())) {
		cout << "Got merge info" << endl;
		this->info = *info;
	} else if (LimitSwitchStatus *limitstatus = dynamic_cast<LimitSwitchStatus *>(dobj.get())) {
		cout << "Got limit switch status" << endl;
		if (limitstatus->getStatus())
			this->info.setFlags(this->info.getFlags() | (1 << 8));
		else
			this->info.setFlags(this->info.getFlags() & ~(1 << 8));
	}
}

void MergeManager::halStateChangeCallback() {
	if (endpoint->getState() == Endpoint::OPEN) {
		endpoint->write(HeartBeat());
		endpoint->write(StartPublishing(10));
	}
}

void MergeManager::halActuatorStateChangeCallback() {

}

void MergeManager::setActuators(int flags) {
	actuatorendpoint->write(SetActuator(flags));
}

