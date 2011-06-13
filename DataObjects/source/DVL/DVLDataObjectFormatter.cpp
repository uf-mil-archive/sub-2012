#include "DataObjects/DVL/DVLDataObjectFormatter.h"
#include "DataObjects/DVL/DVLCommand.h"
#include "DataObjects/DVL/DVLHighresBottomTrack.h"
#include "DataObjects/DVL/DVLBottomTrackRange.h"
#include <memory>

using namespace subjugator;
using namespace std;

DVLDataObjectFormatter::DVLDataObjectFormatter() { }

DataObject *DVLDataObjectFormatter::toDataObject(const Packet &packet) {
	if (packet.size() < 2)
		return NULL;

	if (packet[1] == 0x58) {
		if (packet[0] == 0x03) {
			auto_ptr<DVLHighresBottomTrack> highresptr(new DVLHighresBottomTrack());
			if (DVLHighresBottomTrack::parse(packet.begin(), packet.end(), *highresptr))
				return highresptr.release();
		} else if (packet[0] == 0x04) {
			auto_ptr<DVLBottomTrackRange> rangeptr(new DVLBottomTrackRange());
			if (DVLBottomTrackRange::parse(packet.begin(), packet.end(), *rangeptr))
				return rangeptr.release();
		}
	}

	return NULL;
}

Packet DVLDataObjectFormatter::toPacket(const DataObject &dobj) {
	return dynamic_cast<const DVLCommand &>(dobj).makePacket();
}

