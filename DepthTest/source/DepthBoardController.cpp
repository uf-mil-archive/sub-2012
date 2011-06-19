#include "DepthTest/DepthBoardController.h"
#include "DataObjects/DepthBoard/DepthBoardDataObjectFormatter.h"
#include "DataObjects/DepthBoard/SetReference.h"
#include "DataObjects/DepthBoard/DepthBoardInfo.h"
#include "DataObjects/Embedded/StartPublishing.h"
#include "DataObjects/HeartBeat.h"
#include "HAL/format/Sub7EPacketFormatter.h"
#include <boost/bind.hpp>

using namespace subjugator;
using namespace boost::asio;
using namespace boost;
using namespace std;

DepthBoardController::DepthBoardController(int depthaddr)
: endpoint(hal.openDataObjectEndpoint(depthaddr, new DepthBoardDataObjectFormatter(depthaddr, GUMSTIX_ADDR, DEPTHBOARD), new Sub7EPacketFormatter())),
  heartbeatsender(hal.getIOService(), *endpoint, 2){
	endpoint->configureCallbacks(bind(&DepthBoardController::endpointReadCallback, this, _1), bind(&DepthBoardController::endpointStateChangeCallback, this));
	endpoint->open();
	hal.startIOThread();
}

void DepthBoardController::setReference(double reference) {
	endpoint->write(SetReference(reference));
}

void DepthBoardController::endpointReadCallback(auto_ptr<DataObject> &dobj) {
	if (const DepthBoardInfo *info = dynamic_cast<const DepthBoardInfo *>(dobj.get())) {
		depthinfo = *info;
		emit newInfo();
	}
}

void DepthBoardController::endpointStateChangeCallback() {
	if (endpoint->getState() == Endpoint::OPEN) {
		endpoint->write(HeartBeat());
		endpoint->write(StartPublishing(100));

		heartbeatsender.start();
	} else {
		heartbeatsender.stop();
	}
}
