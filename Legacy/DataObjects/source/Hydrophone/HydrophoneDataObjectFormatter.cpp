#include "DataObjects/Hydrophone/HydrophoneDataObjectFormatter.h"
#include "DataObjects/Hydrophone/HydrophoneSamples.h"
#include "DataObjects/Hydrophone/HydrophoneStart.h"

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

