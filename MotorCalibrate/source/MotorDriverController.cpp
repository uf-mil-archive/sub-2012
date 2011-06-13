#include "MotorCalibrate/MotorDriverController.h"
#include "DataObjects/MotorDriver/MotorDriverDataObjectFormatter.h"
#include "DataObjects/MotorDriver/SetReference.h"
#include "DataObjects/MotorDriver/MotorDriverInfo.h"
#include "DataObjects/Embedded/StartPublishing.h"
#include "DataObjects/HeartBeat.h"
#include "HAL/format/Sub7EPacketFormatter.h"
#include <boost/bind.hpp>

using namespace subjugator;
using namespace boost::asio;
using namespace boost;
using namespace std;

#define DEST_ADDR 108
#define SOURCE_ADDR 112

MotorDriverController::MotorDriverController(int motaddr)
: endpoint(hal.openDataObjectEndpoint(motaddr, new MotorDriverDataObjectFormatter(DEST_ADDR, SOURCE_ADDR, BRUSHEDOPEN), new Sub7EPacketFormatter())),
  heartbeatsender(hal.getIOService(), *endpoint, 2),
  motorramper(hal.getIOService(), *endpoint),
  motorbangbang(hal.getIOService(), *endpoint) {
	endpoint->configureCallbacks(bind(&MotorDriverController::endpointReadCallback, this, _1), bind(&MotorDriverController::endpointStateChangeCallback, this));
	endpoint->open();
	motorramper.configureCallbacks(bind(&MotorDriverController::rampUpdateCallback, this, _1), bind(&MotorDriverController::rampCompleteCallback, this));
	motorbangbang.configureCallbacks(bind(&MotorDriverController::bangUpdateCallback, this, _1));
	hal.startIOThread();
}

void MotorDriverController::setReference(double reference) {
	endpoint->write(SetReference(reference));
}

void MotorDriverController::startRamp(const MotorRamper::Settings &settings) {
	motorramper.start(settings);
}

void MotorDriverController::stopRamp() {
	motorramper.stop();
	endpoint->write(SetReference(0));
	emit newRampReference(0);
}

void MotorDriverController::startBangBang(const MotorBangBang::Settings &settings) {
	motorbangbang.start(settings);
}

void MotorDriverController::stopBangBang() {
	motorbangbang.stop();
	endpoint->write(SetReference(0));
	emit newBangReference(0);
}

void MotorDriverController::endpointReadCallback(auto_ptr<DataObject> &dobj) {
	if (const MotorDriverInfo *info = dynamic_cast<const MotorDriverInfo *>(dobj.get())) {
		motorinfo = *info;
		emit newInfo();
	}
}

void MotorDriverController::endpointStateChangeCallback() {
	if (endpoint->getState() == Endpoint::OPEN) {
		endpoint->write(HeartBeat());
		endpoint->write(StartPublishing(100));

		heartbeatsender.start();
	} else {
		heartbeatsender.stop();
	}
}

void MotorDriverController::rampUpdateCallback(double reference) {
	emit newRampReference(reference);
}

void MotorDriverController::rampCompleteCallback() {
	emit rampComplete();
}

void MotorDriverController::bangUpdateCallback(double reference) {
	emit newBangReference(reference);
}

