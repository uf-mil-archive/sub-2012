#include "DepthTest/DepthController.h"
#include "DataObjects/Depth/DepthDataObjectFormatter.h"
#include "DataObjects/Depth/DepthInfo.h"
#include "DataObjects/Embedded/StartPublishing.h"
#include "DataObjects/EmbeddedAddresses.h"
#include "DataObjects/HeartBeat.h"
#include "HAL/format/Sub7EPacketFormatter.h"
#include <boost/bind.hpp>

using namespace subjugator;
using namespace boost::asio;
using namespace boost;
using namespace std;

DepthController::DepthController(int depthaddr)
: endpoint(hal.openDataObjectEndpoint(depthaddr, new DepthDataObjectFormatter(DEPTH_ADDR, GUMSTIX_ADDR, DEPTH), new Sub7EPacketFormatter())),
  heartbeatsender(hal.getIOService(), *endpoint, 2){
	endpoint->configureCallbacks(bind(&DepthController::endpointReadCallback, this, _1), bind(&DepthController::endpointStateChangeCallback, this));
	endpoint->open();
	hal.startIOThread();
}

void DepthController::endpointReadCallback(auto_ptr<DataObject> &dobj) {
	if (const DepthInfo *info = dynamic_cast<const DepthInfo *>(dobj.get())) {
		depthinfo = *info;
		emit newInfo();
	}
}

void DepthController::endpointStateChangeCallback() {
	if (endpoint->getState() == Endpoint::OPEN) {
		endpoint->write(HeartBeat());
		endpoint->write(StartPublishing(100));

		heartbeatsender.start();
	} else {
		heartbeatsender.stop();
	}
}
