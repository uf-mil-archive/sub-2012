#include "Hydrophone/DataObjects/HydrophoneDataObjectFormatter.h"
#include "Hydrophone/DataObjects/HydrophoneSamples.h"
#include "Hydrophone/DataObjects/HydrophoneStart.h"

using namespace subjugator;
using namespace boost;

HydrophoneDataObjectFormatter::HydrophoneDataObjectFormatter() { }

DataObject *HydrophoneDataObjectFormatter::toDataObject(const Packet &packet) {
	return HydrophoneSamples::parse(packet.begin(), packet.end());
}

Packet HydrophoneDataObjectFormatter::toPacket(const DataObject &dobj) {
	if (dynamic_cast<const HydrophoneStart *>(&dobj)) {
		return HydrophoneStart::startbytes;
	}

	return Packet();
}

