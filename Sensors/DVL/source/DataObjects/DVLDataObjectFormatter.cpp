#include "DVL/DataObjects/DVLDataObjectFormatter.h"
#include "DVL/DataObjects/DVLCommand.h"
#include "DVL/DataObjects/DVLInfo.h"

using namespace subjugator;
using namespace std;

DVLDataObjectFormatter::DVLDataObjectFormatter() { }

DataObject *DVLDataObjectFormatter::toDataObject(const Packet &packet) {
	if (packet.size() < 40) // eh, this is ugly. PacketFormatter and DataObjectFormatter distinction only really exists for embedded protocol
		return new DVLStartupBanner();
	else
		return new DVLInfo(packet.begin(), packet.end());
}

Packet DVLDataObjectFormatter::toPacket(const DataObject &dobj) {
	return dynamic_cast<const DVLCommand &>(dobj).makePacket();
}

