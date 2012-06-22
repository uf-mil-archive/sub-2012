#include "PrimitiveDriver/DataObjects/ActuatorDataObjectFormatter.h"
#include "PrimitiveDriver/DataObjects/ActuatorDataObjects.h"
#include <cassert>

using namespace subjugator;
using namespace boost;
using namespace std;

template <typename DObj>
static bool match(uint8_t cmd) {
	return (cmd & DObj::CMD_MASK) == DObj::CMD_NUMBER;
}

DataObject *ActuatorDataObjectFormatter::toDataObject(const Packet &packet) {
	assert(packet.size() == 2);

	uint8_t cmd = packet[0];
	if (match<Ping>(cmd))
		return new Ping();
	else if (match<SetValves>(cmd))
		return new SetValves(cmd);
	else if (match<SwitchValues>(cmd))
		return new SwitchValues(cmd);
	else
		return NULL;
}

Packet ActuatorDataObjectFormatter::toPacket(const DataObject &dobj) {
	uint8_t cmd=0;
	if (dynamic_cast<const Ping *>(&dobj)) {
		cmd = Ping::CMD_NUMBER;
	} else if (const SetValves *valves = dynamic_cast<const SetValves *>(&dobj)) {
		cmd = SetValves::CMD_NUMBER | (uint8_t)valves->bits.to_ulong();
	} else if (dynamic_cast<const ReadSwitches *>(&dobj)) {
		cmd = ReadSwitches::CMD_NUMBER;
	}

	Packet packet;
	packet.push_back(cmd);
	return packet;
}
