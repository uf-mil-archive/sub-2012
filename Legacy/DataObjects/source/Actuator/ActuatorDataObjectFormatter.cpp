#include "DataObjects/Actuator/ActuatorDataObjectFormatter.h"
#include "DataObjects/Actuator/LimitSwitchStatus.h"
#include "DataObjects/Actuator/SetActuator.h"
#include <cassert>

using namespace subjugator;

ActuatorDataObjectFormatter::ActuatorDataObjectFormatter() { }

DataObject *ActuatorDataObjectFormatter::toDataObject(const Packet &packet) {
	assert(packet.size() > 0);
	return new LimitSwitchStatus(packet[0]);
}

Packet ActuatorDataObjectFormatter::toPacket(const DataObject &dobj) {
	if (const SetActuator *setactuator = dynamic_cast<const SetActuator *>(&dobj)) {
		return Packet(1, setactuator->getFlags());
	}

	return Packet();
}

