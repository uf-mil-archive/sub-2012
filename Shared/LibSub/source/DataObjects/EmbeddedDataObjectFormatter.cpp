#include "LibSub/DataObjects/EmbeddedDataObjectFormatter.h"
#include "LibSub/DataObjects/EmbeddedCommand.h"

using namespace subjugator;
using namespace boost;

EmbeddedDataObjectFormatter::EmbeddedDataObjectFormatter(boost::uint8_t devaddress, boost::uint8_t pcaddress, EmbeddedTypeCode typecode)
: devaddress(devaddress), pcaddress(pcaddress), typecode(typecode), packetcount_out(0), packetcount_in(0) { }

DataObject *EmbeddedDataObjectFormatter::toDataObject(const Packet &packet) {
	// verify header stuff
	if (packet[0] != devaddress || packet[1] != pcaddress || packet[4] != typecode)
		return NULL;

	// TODO packet number stats

	// create the data object
	return makeInfoDataObject(packet.begin()+5, packet.end());
}

Packet EmbeddedDataObjectFormatter::toPacket(const DataObject &dobj) {
	Packet packet;
	packet.push_back(pcaddress);
	packet.push_back(devaddress);
	packet.push_back((uint8_t)(packetcount_out & 0xFF));
	packet.push_back((uint8_t)(packetcount_out >> 8));

	packetcount_out++;

	// Leaving this somewhat ugly because I think Heartbeat is going to be completely refactored out of here once multicast gets in place
	if (const EmbeddedCommand *command = dynamic_cast<const EmbeddedCommand *>(&dobj)) {
		packet.push_back(typecode);
		packet.push_back(command->getToken());
		command->appendDataPacket(packet);
	} // TODO error handling here

	return packet;
}

