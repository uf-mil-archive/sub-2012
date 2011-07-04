#ifndef LOCALWAYPOINTDRIVERDDSListener_H
#define LOCALWAYPOINTDRIVERDDSListener_H

#include "DataObjects/LocalWaypointDriver/LocalWaypointDriverInfo.h"

#include "SubMain/Workers/SubWorker.h"
#include "SubMain/Workers/SubListener.h"

#include "DDSMessages/PDWrenchMessage.h"
#include "DDSMessages/PDWrenchMessageSupport.h"
#include "DDSMessages/TrajectoryMessage.h"
#include "DDSMessages/TrajectoryMessageSupport.h"
#include "DDSListeners/DDSSender.h"
#include <ndds/ndds_cpp.h>

namespace subjugator
{
	class LocalWaypointDriverDDSListener : public Listener
	{
	public:
		LocalWaypointDriverDDSListener(Worker &worker, DDSDomainParticipant *part);

	protected:
		virtual void DataObjectEmitted(boost::shared_ptr<DataObject> dobj);

	private:
		DDSSender<PDWrenchMessage, PDWrenchMessageDataWriter, PDWrenchMessageTypeSupport> PDWrenchddssender;
		DDSSender<TrajectoryMessage, TrajectoryMessageDataWriter, TrajectoryMessageTypeSupport> Trajectoryddssender;

	};
}

#endif // LocalWaypointDriverDDSListener_H
