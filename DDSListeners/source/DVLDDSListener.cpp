#include "DDSListeners/DVLDDSListener.h"

using namespace subjugator;

bool DVLDDSListener::BuildMessage(DVLMessage *msg, DataObject *obj)
{
	// Cast the data object into its real type
	DVLHighresBottomTrack *dvlinfo = dynamic_cast<DVLHighresBottomTrack *>(obj);
	if(!dvlinfo)
	{
		// Not interested in any other object type
		return false;
	}

	msg->timestamp = dvlinfo->getTimestamp();

	for(int i =0; i < 3; i++)
		msg->velocity[i] = dvlinfo->getVelocityI(i);
	msg->velocityerror = dvlinfo->getVelocityError();
	msg->height = 0.0;
	//msg->beamcorrelation = {0.0,0.0,0.0,0.0};

	return true;
}
