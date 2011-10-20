#include "PrimitiveDriver/MergeManager.h"
#include "DataObjects/Merge/MergeDataObjectFormatter.h"
#include "DataObjects/Embedded/StartPublishing.h"
#include "DataObjects/HeartBeat.h"
#include "DataObjects/Actuator/SetActuator.h"
#include "DataObjects/Actuator/ActuatorDataObjectFormatter.h"
#include "DataObjects/Actuator/LimitSwitchStatus.h"
#include "HAL/format/Sub7EPacketFormatter.h"
#include "HAL/format/BytePacketFormatter.h"
#include <boost/bind.hpp>

using namespace subjugator;
using namespace boost;
using namespace std;

MergeManager::MergeManager(SubHAL &hal)
: mergeendpoint(hal.openDataObjectEndpoint(60, new MergeDataObjectFormatter(60, 21), new Sub7EPacketFormatter()),
               "merge",
               boost::bind(&MergeManager::mergeInitCallback, this),
               false, .5),
  actuatorendpoint(hal.openDataObjectEndpoint(61, new ActuatorDataObjectFormatter(), new BytePacketFormatter()),
               "actuator",
               boost::bind(&MergeManager::actuatorInitCallback, this),
               false, .5) {
	registerStateUpdater(mergeendpoint);
	registerStateUpdater(actuatorendpoint);
}

MergeInfo MergeManager::getMergeInfo() const {
	MergeInfo info = *mergeendpoint.getDataObject<MergeInfo>(); // TODO this will get fixed up in the actuator overhaul
	if (actuatorendpoint.getDataObject<LimitSwitchStatus>()->getStatus())
		info.setFlags(info.getFlags() | (1 << 8));
	return info;
}

void MergeManager::setActuators(int flags) {
	actuatorendpoint.write(SetActuator(flags));
}

void MergeManager::mergeInitCallback() {
	mergeendpoint.write(HeartBeat());
	mergeendpoint.write(StartPublishing(10));
}

void MergeManager::actuatorInitCallback() {
	actuatorendpoint.write(SetActuator(0));
}

