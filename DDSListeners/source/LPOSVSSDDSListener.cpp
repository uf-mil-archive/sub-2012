#include "DDSListeners/LPOSVSSDDSListener.h"
#include "DDSMessages/LPOSVSSMessage.h"

using namespace subjugator;

LPOSVSSDDSListener::LPOSVSSDDSListener(Worker &worker, DDSDomainParticipant *part)
: Listener(worker), ddssender(part, "LPOSVSS") { }

void LPOSVSSDDSListener::DataObjectEmitted(boost::shared_ptr<DataObject> dobj)
{
	// Cast the data object into its real type
	LPOSVSSInfo *info = dynamic_cast<LPOSVSSInfo *>(dobj.get());
	if (!info)
		return;

	LPOSVSSMessage *msg = LPOSVSSMessageTypeSupport::create_data();
	msg->timestamp = info->getTimestamp();

	for (int i=0; i<3; i++)
		msg->position_NED[i] = info->getPosition_NED()(i);

	for (int i=0; i<4; i++)
		msg->quaternion_NED_B[i] = info->getQuat_NED_B()(i);

	for (int i=0; i<3; i++)
		msg->velocity_NED[3] = info->getVelocity_NED()(i);

	for (int i=0; i<3; i++)
		msg->angularRate_BODY[3] = info->getAngularRate_BODY()(i);

	for (int i=0; i<3; i++)
		msg->acceleration_BODY[3] = info->getAcceleration_BODY()(i);

	ddssender.Send(*msg);
	LPOSVSSMessageTypeSupport::delete_data(msg);
}

