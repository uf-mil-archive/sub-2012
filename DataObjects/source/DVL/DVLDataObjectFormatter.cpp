#include "DataObjects/DVL/DVLDataObjectFormatter.h"
#include "DataObjects/DVL/DVLCommand.h"
#include "DataObjects/DVL/DVLHighresBottomTrack.h"
#include "DataObjects/DVL/DVLBottomTrackRange.h"
#include "DataObjects/DVL/DVLBottomTrack.h"
#include <memory>

using namespace subjugator;
using namespace std;

DVLDataObjectFormatter::DVLDataObjectFormatter() { }

DataObject *DVLDataObjectFormatter::toDataObject(const Packet &packet) {
	if (packet.size() < 2)
		return NULL;

	DataObject *dobj;
	if ((dobj = DVLBottomTrack::parse(packet.begin(), packet.end())) != NULL)
		return dobj;
	if ((dobj = DVLHighresBottomTrack::parse(packet.begin(), packet.end())) != NULL)
		return dobj;
	if ((dobj = DVLBottomTrackRange::parse(packet.begin(), packet.end())) != NULL)
		return dobj;

	return NULL;
}

Packet DVLDataObjectFormatter::toPacket(const DataObject &dobj) {
	return dynamic_cast<const DVLCommand &>(dobj).makePacket();
}

