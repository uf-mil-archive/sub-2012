#include "DataObjects/DVL/DVLDataObjectFormatter.h"
#include "DataObjects/DVL/DVLCommand.h"

using namespace subjugator;

DVLDataObjectFormatter::DVLDataObjectFormatter() { }

DataObject *DVLDataObjectFormatter::toDataObject(const Packet &packet) {
	return NULL; // TODO
}

Packet DVLDataObjectFormatter::toPacket(const DataObject &dobj) {
	return dynamic_cast<const DVLCommand &>(dobj).makePacket();
}

