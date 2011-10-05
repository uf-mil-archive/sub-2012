#include "EquTrajectoryGenerator/TrajectoryGeneratorDDSListener.h"

using namespace subjugator;

TrajectoryGeneratorDDSListener::TrajectoryGeneratorDDSListener(Worker &worker, DDSDomainParticipant *part)
: trajectoryddssender(part, "Trajectory")
  { connectWorker(worker); }

void TrajectoryGeneratorDDSListener::DataObjectEmitted(boost::shared_ptr<DataObject> dobj)
{
	TrajectoryInfo *trajInfo;
	if ((trajInfo = dynamic_cast<TrajectoryInfo *>(dobj.get())) == NULL)
		return;

	TrajectoryMessage *msg = TrajectoryMessageTypeSupport::create_data();

	msg->timestamp = trajInfo->getTimestamp();
	for (int i=0;i<6;i++) {
		msg->xd[i] = trajInfo->getTrajectory()(i);
		msg->xd_dot[i] = trajInfo->getTrajectory_dot()(i);
	}

	trajectoryddssender.Send(*msg);
	TrajectoryMessageTypeSupport::delete_data(msg);
}

