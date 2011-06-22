#include "DDSListeners/DVLDDSListener.h"

using namespace subjugator;

void DVLDDSListener::BuildMessage(DVLMessage *msg, DataObject *obj)
{
	// Cast the data object into its real type
	DVLHighresBottomTrack *dvlinfo = dynamic_cast<DVLHighresBottomTrack *>(obj);
	if(!dvlinfo)
		return;

	msg->timestamp = dvlinfo->getTimestamp();

	for(int i =0; i < 3; i++)
		msg->velocity[i] = dvlinfo->getVelocityI(i);
	msg->velocityerror = dvlinfo->getVelocityError();
	msg->height = 0.0;
	//msg->beamcorrelation = {0.0,0.0,0.0,0.0};

}
