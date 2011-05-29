#include "MotorDriverDataObjectFormatter.h"
#include "HeartBeat.h"

using namespace subjugator;

MotorDriverDataObjectFormatter::MotorDriverDataObjectFormatter()
: packetcount_out(0), packetcount_in(0) { }

DataObject *MotorDriverDataObjectFormatter::toDataObject(const Packet &packet) {
	return NULL;
}

Packet MotorDriverDataObjectFormatter::toPacket(const DataObject &dobj) {
	Packet packet;

	if (const HeartBeat *hb = dynamic_cast<const HeartBeat *>(&dobj)) {
		packet.push_back(hb->getAddress());
		packet.push_back((uint8_t)(packetcount_out & 0xFF));
		packet.push_back((uint8_t)(packetcount_out >> 8));
		packet.push_back(hb->getTypeCode());
		packetcount_out++;
		return packet;
	}
}

