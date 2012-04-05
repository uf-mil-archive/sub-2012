#include "PrimitiveDriver/MergeManager.h"
#include "PrimitiveDriver/DataObjects/MergeDataObjectFormatter.h"
#include "PrimitiveDriver/DataObjects/HeartBeat.h"
#include "LibSub/DataObjects/StartPublishing.h"
#include "HAL/format/Sub7EPacketFormatter.h"
#include <boost/bind.hpp>

using namespace subjugator;
using namespace boost;
using namespace std;

MergeManager::MergeManager(HAL &hal, const string &mergeendpointconf, const EStopUpdateCallback &estopupdatecallback) :
	estopupdatecallback(estopupdatecallback),
	mergeendpoint(WorkerEndpoint::Args()
	              .setName("merge")
	              .setEndpoint(hal.makeDataObjectEndpoint(mergeendpointconf, new MergeDataObjectFormatter(60, 21, MERGEBOARD), new Sub7EPacketFormatter()))
	              .setInitCallback(boost::bind(&MergeManager::mergeInitCallback, this))
	              .setReceiveCallback(boost::bind(&MergeManager::mergeReceiveCallback, this, _1))
	              .setMaxAge(.5)),
	prevestop(false)
{
	registerStateUpdater(mergeendpoint);
}

void MergeManager::mergeInitCallback() {
	mergeendpoint.write(HeartBeat());
	mergeendpoint.write(StartPublishing(10));
}

void MergeManager::mergeReceiveCallback(const boost::shared_ptr<DataObject> &data) {
	bool estop = getMergeInfo().getESTOP();

	if (estop != prevestop) {
		estopupdatecallback(estop);
		prevestop = estop;
	}
}

