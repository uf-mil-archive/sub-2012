#include "MotorCalibrate/MotorDriverController.h"
#include "DataObjects/MotorDriver/MotorDriverDataObjectFormatter.h"
#include "DataObjects/MotorDriver/SetReference.h"
#include "DataObjects/MotorDriver/MotorDriverInfo.h"
#include "DataObjects/MotorDriver/StartPublishing.h"
#include "DataObjects/HeartBeat.h"
#include "HAL/format/Sub7EPacketFormatter.h"
#include <boost/bind.hpp>

using namespace subjugator;
using namespace boost::asio;
using namespace boost;
using namespace std;

MotorDriverController::MotorDriverController(int motaddr)
: endpoint(hal.openDataObjectEndpoint(motaddr, new MotorDriverDataObjectFormatter(motaddr, 1, BRUSHEDOPEN), new Sub7EPacketFormatter())),
  heartbeatsender(hal.getIOService(), *endpoint) {
	endpoint->configureCallbacks(bind(&MotorDriverController::endpointReadCallback, this, _1), bind(&MotorDriverController::endpointStateChangeCallback, this));
	hal.startIOThread();
}

void MotorDriverController::setReference(double reference) {
	endpoint->write(SetReference(reference));
}

void MotorDriverController::endpointReadCallback(auto_ptr<DataObject> &dobj) {
	if (const MotorDriverInfo *info = dynamic_cast<const MotorDriverInfo *>(dobj.get())) {
		emit newInfo(*info);
	}
}

void MotorDriverController::endpointStateChangeCallback() {
	if (endpoint->getState() == Endpoint::OPEN) {
		endpoint->write(HeartBeat());
		endpoint->write(StartPublishing(10));

		heartbeatsender.start();
	} else {
		heartbeatsender.stop();
	}
}

