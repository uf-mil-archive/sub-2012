#include "PrimitiveDriver/ActuatorManager.h"
#include "PrimitiveDriver/DataObjects/ActuatorDataObjects.h"
#include "PrimitiveDriver/DataObjects/ActuatorDataObjectFormatter.h"
#include "PrimitiveDriver/DataObjects/ActuatorPacketFormatter.h"
#include <algorithm>
#include <boost/bind.hpp>

using namespace subjugator;
using namespace boost;
using namespace std;

ActuatorManager::ActuatorManager(HAL &hal, const std::string &actuatorendpointconf) :
	endpoint(WorkerEndpoint::Args()
	         .setName("actuator")
	         .setEndpoint(hal.makeDataObjectEndpoint(actuatorendpointconf, new ActuatorDataObjectFormatter(), new ActuatorPacketFormatter()))
	         .setReceiveCallback(boost::bind(&ActuatorManager::actuatorReceiveCallback, this, _1))
	         .setMaxAge(.5)),
	actuators(ACTUATOR_COUNT, false),
	switches(SWITCH_COUNT, false),
	switchreadtimer(0)
{
	registerStateUpdater(endpoint);
	sendSetValves();
}

void ActuatorManager::updateState(double dt) {
	StateUpdaterContainer::updateState(dt);

	switchreadtimer += dt;
	if (switchreadtimer > .2) {
		switchreadtimer = 0;
		sendReadSwitches();
	}
}

void ActuatorManager::setActuator(int num, bool state) {
	actuators[num] = state;
	sendSetValves();
}

void ActuatorManager::setActuators(const vector<bool> &actuators) {
	assert(actuators.size() == ACTUATOR_COUNT);
	this->actuators = actuators;
	sendSetValves();
}

void ActuatorManager::offActuators() {
	fill(actuators.begin(), actuators.end(), false);
	sendSetValves();
}

void ActuatorManager::actuatorReceiveCallback(const boost::shared_ptr<DataObject> &data) {
	if (boost::shared_ptr<SwitchValues> switchvals = dynamic_pointer_cast<SwitchValues>(data)) {
		switches[0] = switchvals->in1;
		switches[1] = switchvals->in2;
	}
}

void ActuatorManager::sendSetValves() {
	uint8_t bits=0;
	for (int i=0; i<ACTUATOR_COUNT; i++) {
		if (actuators[i])
			bits |= (1 >> i);
	}
	endpoint.write(SetValves(bits));
}

void ActuatorManager::sendReadSwitches() {
	endpoint.write(ReadSwitches());
}
