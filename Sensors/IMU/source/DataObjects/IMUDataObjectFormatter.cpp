#include "IMU/DataObjects/IMUDataObjectFormatter.h"
#include "IMU/DataObjects/IMUInfo.h"
#include <memory>

using namespace subjugator;
using namespace boost;

IMUDataObjectFormatter::IMUDataObjectFormatter() { }

DataObject *IMUDataObjectFormatter::toDataObject(const Packet &packet) {
	std::auto_ptr<IMUInfo> info(new IMUInfo());
	if (IMUInfo::parse(*info, packet.begin(), packet.end()))
		return info.release();
	else
		return NULL;
}

Packet IMUDataObjectFormatter::toPacket(const DataObject &dobj) {
	return Packet();
}
