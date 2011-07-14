#include "DDSListeners/MissionPlannerDDSListener.h"
#include "DDSMessages/SetWaypointMessage.h"
#include "DataObjects/Waypoint/Waypoint.h"
#include "DDSMessages/VisionSetIDsMessage.h"
#include "DataObjects/Vision/VisionSetIDs.h"
#include "DDSMessages/PDActuatorMessage.h"
#include "DDSMessages/PDActuatorMessageSupport.h"
#include "DataObjects/Actuator/SetActuator.h"

using namespace subjugator;

MissionPlannerDDSListener::MissionPlannerDDSListener(Worker &worker, DDSDomainParticipant *part)
: waypointddssender(part, "SetWaypoint"),
  visionidsddssender(part, "VisionSetIDs"),
  pdactuatorddssender(part, "PDActuator") {
	connectWorker(worker);
}

void MissionPlannerDDSListener::DataObjectEmitted(boost::shared_ptr<DataObject> dobj)
{
	if (Waypoint *waypoint = dynamic_cast<Waypoint *>(dobj.get())) {
		SetWaypointMessage *msg = SetWaypointMessageTypeSupport::create_data();
		msg->isRelative = waypoint->isRelative;
		//msg->number = waypoint->number;

		for (int i=0; i<3; i++)
			msg->position_ned[i] = waypoint->Position_NED(i);

		for (int i=0; i<3; i++)
			msg->rpy[i] = waypoint->RPY(i);

		waypointddssender.Send(*msg);
		SetWaypointMessageTypeSupport::delete_data(msg);
	} else if (VisionSetIDs *setids = dynamic_cast<VisionSetIDs *>(dobj.get())) {
		VisionSetIDsMessage *msg = VisionSetIDsMessageTypeSupport::create_data();

		int ids = setids->getIDs().size();
		msg->visionids.ensure_length(ids, ids);
		for (int i=0; i<ids; i++) {
			msg->visionids[i] = setids->getIDs()[i];
		}
		msg->cameraid = setids->getCameraID();

		visionidsddssender.Send(*msg);
		VisionSetIDsMessageTypeSupport::delete_data(msg);
	} else if (SetActuator *act = dynamic_cast<SetActuator *>(dobj.get())) {
		PDActuatorMessage *actuator = PDActuatorMessageTypeSupport::create_data();

		actuator->flags = act->getFlags();

		pdactuatorddssender.Send(*actuator);
		PDActuatorMessageTypeSupport::delete_data(actuator);
	}
}
