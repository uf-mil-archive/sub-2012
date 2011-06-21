#include "DDSListeners/IMUDDSListener.h"

using namespace subjugator;

void IMUDDSListener::BuildMessage(IMUMessage *msg, DataObject *obj)
{
	// Cast the data object into its real type
	IMUInfo *imuinfo = dynamic_cast<IMUInfo *>(obj);
	if(!imuinfo)
		return;

	msg->timestamp = imuinfo->getTimestamp();
	msg->flags = imuinfo->getFlags();
	msg->supply = imuinfo->getSupplyVoltage();

	for(int i = 0; i < 3; i++)
	{
		msg->acceleration[i] = imuinfo->getAccelerationI(i);
		msg->mag_field[i] = imuinfo->getMagneticFieldI(i);
		msg->angular_rate[i] = imuinfo->getAngularRateI(i);
	}
}
