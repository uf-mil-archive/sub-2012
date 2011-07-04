#include "DDSListeners/LocalWaypointDriverDDSListener.h"

using namespace subjugator;

LocalWaypointDriverDDSListener::LocalWaypointDriverDDSListener(Worker &worker, DDSDomainParticipant *part)
: PDWrenchddssender(part, "PDWrench"),
  Trajectoryddssender(part, "Trajectory")
  { connectWorker(worker); }

void LocalWaypointDriverDDSListener::DataObjectEmitted(boost::shared_ptr<DataObject> dobj)
{
	// Cast the data object into its real type
	LocalWaypointDriverInfo *localwaypointdriverinfo = dynamic_cast<LocalWaypointDriverInfo *>(dobj.get());
	if (!localwaypointdriverinfo)
		return;

	cout << "Message To Send" << endl;

	PDWrenchMessage *msg1 = PDWrenchMessageTypeSupport::create_data();

	msg1->linear[0] = localwaypointdriverinfo->Wrench[0];
	msg1->linear[1] = localwaypointdriverinfo->Wrench[1];
	msg1->linear[2] = localwaypointdriverinfo->Wrench[2];
	msg1->moment[0] = localwaypointdriverinfo->Wrench[3];
	msg1->moment[1] = localwaypointdriverinfo->Wrench[4];
	msg1->moment[2] = localwaypointdriverinfo->Wrench[5];

	TrajectoryMessage *msg2 = TrajectoryMessageTypeSupport::create_data();

	msg2->trajectory[0] = localwaypointdriverinfo->X[0];
	msg2->trajectory[1] = localwaypointdriverinfo->X[1];
	msg2->trajectory[2] = localwaypointdriverinfo->X[2];
	msg2->trajectory[3] = localwaypointdriverinfo->X[3];
	msg2->trajectory[4] = localwaypointdriverinfo->X[4];
	msg2->trajectory[5] = localwaypointdriverinfo->X[5];

	msg2->trajectory_dot[0] = localwaypointdriverinfo->Xd[0];
	msg2->trajectory_dot[1] = localwaypointdriverinfo->Xd[1];
	msg2->trajectory_dot[2] = localwaypointdriverinfo->Xd[2];
	msg2->trajectory_dot[3] = localwaypointdriverinfo->Xd[3];
	msg2->trajectory_dot[4] = localwaypointdriverinfo->Xd[4];
	msg2->trajectory_dot[5] = localwaypointdriverinfo->Xd[5];

	PDWrenchddssender.Send(*msg1);
	Trajectoryddssender.Send(*msg2);

	PDWrenchMessageTypeSupport::delete_data(msg1);
	TrajectoryMessageTypeSupport::delete_data(msg2);
}

