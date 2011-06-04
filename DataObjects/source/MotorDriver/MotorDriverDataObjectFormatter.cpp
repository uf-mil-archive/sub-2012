#include "DataObjects/MotorDriver/MotorDriverDataObjectFormatter.h"
#include "DataObjects/MotorDriver/MotorDriverDataObject.h"

using namespace subjugator;
using namespace boost;

MotorDriverDataObjectFormatter::MotorDriverDataObjectFormatter(uint8_t address)
: address(address), packetcount_out(0), packetcount_in(0) { }

DataObject *MotorDriverDataObjectFormatter::toDataObject(const Packet &packet) {
	return NULL;
}

Packet MotorDriverDataObjectFormatter::toPacket(const DataObject &dobj) {
	const MotorDriverDataObject &motdobj = dynamic_cast<const MotorDriverDataObject &>(dobj);

	Packet packet;
	packet.push_back(address);
	packet.push_back((uint8_t)(packetcount_out & 0xFF));
	packet.push_back((uint8_t)(packetcount_out >> 8));
	packet.push_back(motdobj.getTypeCode());
	motdobj.appendData(packet);

	packetcount_out++;
	return packet;
}

