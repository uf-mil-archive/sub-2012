#include "DDSListeners/IMUDDSListener.h"

using namespace subjugator;

IMUDDSListener::IMUDDSListener(Worker &worker, DDSDomainParticipant *part)
: Listener(worker), ddssender(part, "IMU") { }

void IMUDDSListener::DataObjectEmitted(boost::shared_ptr<DataObject> dobj)
{
	// Cast the data object into its real type
	IMUInfo *imuinfo = dynamic_cast<IMUInfo *>(dobj.get());
	if (!imuinfo)
		return;

	IMUMessage *msg = IMUMessageTypeSupport::create_data();
	msg->timestamp = imuinfo->getTimestamp();
	msg->flags = imuinfo->getFlags();
	msg->supply = imuinfo->getSupplyVoltage();
	msg->temp = imuinfo->getTemperature();

	for(int i = 0; i < 3; i++)
	{
		msg->acceleration[i] = imuinfo->getAccelerationI(i);
		msg->mag_field[i] = imuinfo->getMagneticFieldI(i);
		msg->angular_rate[i] = imuinfo->getAngularRateI(i);
	}

	ddssender.Send(*msg);
	IMUMessageTypeSupport::delete_data(msg);
}

