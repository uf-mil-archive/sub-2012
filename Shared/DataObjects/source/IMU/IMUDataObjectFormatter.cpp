#include "DataObjects/IMU/IMUDataObjectFormatter.h"
#include "DataObjects/IMU/IMUInfo.h"

using namespace subjugator;
using namespace boost;


IMUDataObjectFormatter::IMUDataObjectFormatter()
{

}


DataObject *IMUDataObjectFormatter::toDataObject(const Packet & packet)
{
	return IMUInfo::parse(packet.begin(), packet.end());
}

Packet IMUDataObjectFormatter::toPacket(const DataObject &dobj)
{
	return Packet();
}

