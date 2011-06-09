#include "DataObjects/MotorDriver/MotorDriverDataObjectFormatter.h"
#include "DataObjects/MotorDriver/MotorDriverCommand.h"
#include "DataObjects/MotorDriver/MotorDriverInfo.h"
#include "DataObjects/HeartBeat.h"

using namespace subjugator;
using namespace boost;

MotorDriverDataObjectFormatter::MotorDriverDataObjectFormatter(boost::uint8_t devaddress, boost::uint8_t pcaddress, EmbeddedTypeCode typecode)
: devaddress(devaddress), pcaddress(pcaddress), typecode(typecode), packetcount_out(0), packetcount_in(0) { }

DataObject *MotorDriverDataObjectFormatter::toDataObject(const Packet &packet) {
	// motor drivers only send back one kind of packet
	if (packet.size() != MotorDriverInfo::Length + 5) // 5 header bytes
		return NULL;

	// verify header stuff
	if (packet[0] != devaddress || packet[1] != pcaddress || packet[4] != typecode)
		return NULL;

	// TODO packet number stats

	// create the new packet
	return new MotorDriverInfo(packet.begin()+5, packet.end());
}

Packet MotorDriverDataObjectFormatter::toPacket(const DataObject &dobj) {
	Packet packet;
	packet.push_back(pcaddress);
	packet.push_back(devaddress);
	packet.push_back((uint8_t)(packetcount_out & 0xFF));
	packet.push_back((uint8_t)(packetcount_out >> 8));

	packetcount_out++;

	// Leaving this somewhat ugly because I think Heartbeat is going to be completely refactored out of here once multicast gets in place
	if (dynamic_cast<const HeartBeat *>(&dobj)) {
		packet.push_back(HeartBeat::TypeCode);
	} else if (const MotorDriverCommand *command = dynamic_cast<const MotorDriverCommand *>(&dobj)) {
		packet.push_back(typecode);
		packet.push_back(command->getToken());
		command->appendDataPacket(packet);
	}

	return packet;
}

