#ifndef LOCALWAYPOINTDRIVERDDSListener_H
#define LOCALWAYPOINTDRIVERDDSListener_H

#include "DataObjects/Trajectory/TrajectoryInfo.h"

#include "SubMain/Workers/SubWorker.h"
#include "SubMain/Workers/SubListener.h"

#include "DDSMessages/TrajectoryMessage.h"
#include "DDSMessages/TrajectoryMessageSupport.h"
#include "DDSListeners/DDSSender.h"
#include <ndds/ndds_cpp.h>

namespace subjugator
{
	class TrajectoryGeneratorDDSListener : public Listener
	{
	public:
		TrajectoryGeneratorDDSListener(Worker &worker, DDSDomainParticipant *part);

	protected:
		virtual void DataObjectEmitted(boost::shared_ptr<DataObject> dobj);

	private:
		DDSSender<TrajectoryMessage, TrajectoryMessageDataWriter, TrajectoryMessageTypeSupport> trajectoryddssender;

	};
}

#endif // TrackingControllerDDSListener_H
