#include "PrimitiveDriver/MergeManager.h"
#include "PrimitiveDriver/DataObjects/MergeDataObjectFormatter.h"
#include "PrimitiveDriver/DataObjects/SetActuator.h"
#include "PrimitiveDriver/DataObjects/ActuatorDataObjectFormatter.h"
#include "PrimitiveDriver/DataObjects/LimitSwitchStatus.h"
#include "PrimitiveDriver/DataObjects/HeartBeat.h"
#include "LibSub/DataObjects/StartPublishing.h"
#include "HAL/format/Sub7EPacketFormatter.h"
#include "HAL/format/BytePacketFormatter.h"
#include <boost/bind.hpp>

using namespace subjugator;
using namespace boost;
using namespace std;

MergeManager::MergeManager(HAL &hal) :
mergeendpoint(WorkerEndpoint::Args()
	.setName("merge")
	.setEndpoint(hal.openDataObjectEndpoint(60, new MergeDataObjectFormatter(60, 21, MERGEBOARD), new Sub7EPacketFormatter()))
	.setInitCallback(boost::bind(&MergeManager::mergeInitCallback, this))
	.setMaxAge(.5)
),
actuatorendpoint(WorkerEndpoint::Args()
	.setName("actuator")
	.setEndpoint(hal.openDataObjectEndpoint(61, new ActuatorDataObjectFormatter(), new BytePacketFormatter()))
	.setInitCallback(boost::bind(&MergeManager::actuatorInitCallback, this))
	.setMaxAge(.5)
) {
	registerStateUpdater(mergeendpoint);
	registerStateUpdater(actuatorendpoint);
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

