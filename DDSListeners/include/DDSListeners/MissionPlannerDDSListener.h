#ifndef DDSLISTENERS_MISSIONPLANNER_H
#define DDSLISTENERS_MISSIONPLANNER_H

#include "SubMain/Workers/SubWorker.h"
#include "SubMain/Workers/SubListener.h"
#include "DDSMessages/SetWaypointMessage.h"
#include "DDSMessages/SetWaypointMessageSupport.h"
#include "DDSMessages/VisionSetIDsMessage.h"
#include "DDSMessages/VisionSetIDsMessageSupport.h"
#include "DDSMessages/PDActuatorMessage.h"
#include "DDSMessages/PDActuatorMessageSupport.h"

#include "DDSListeners/DDSSender.h"

namespace subjugator {
	class MissionPlannerDDSListener : public Listener
	{
	public:
		MissionPlannerDDSListener(Worker &worker, DDSDomainParticipant *part);

	protected:
		virtual void DataObjectEmitted(boost::shared_ptr<DataObject> dobj);

	private:
		DDSSender<SetWaypointMessage, SetWaypointMessageDataWriter, SetWaypointMessageTypeSupport> waypointddssender;
		DDSSender<VisionSetIDsMessage, VisionSetIDsMessageDataWriter, VisionSetIDsMessageTypeSupport> visionidsddssender;
		DDSSender<PDActuatorMessage, PDActuatorMessageDataWriter, PDActuatorMessageTypeSupport> pdactuatorddssender;
	};
}

#endif

