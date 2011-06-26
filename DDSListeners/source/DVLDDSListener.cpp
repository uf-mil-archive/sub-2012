#include "DDSListeners/DVLDDSListener.h"

using namespace subjugator;

DVLDDSListener::DVLDDSListener(Worker &worker, DDSDomainParticipant *part)
: Listener(worker), ddssender(part, "DVL") { }

void DVLDDSListener::DataObjectEmitted(boost::shared_ptr<DataObject> dobj)
{
	// Cast the data object into its real type
	DVLHighresBottomTrack *dvlinfo = dynamic_cast<DVLHighresBottomTrack *>(dobj.get());
	if(!dvlinfo)
		return;

	DVLMessage *msg = DVLMessageTypeSupport::create_data();
	msg->timestamp = dvlinfo->getTimestamp();

	for(int i =0; i < 3; i++)
		msg->velocity[i] = dvlinfo->getVelocityI(i);
	msg->velocityerror = dvlinfo->getVelocityError();
	msg->height = 0.0;
	//msg->beamcorrelation = {0.0,0.0,0.0,0.0};

	ddssender.Send(*msg);
	DVLMessageTypeSupport::delete_data(msg);
}