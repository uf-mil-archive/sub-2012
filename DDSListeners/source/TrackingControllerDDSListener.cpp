#include "DDSListeners/TrackingControllerDDSListener.h"

using namespace subjugator;

TrackingControllerDDSListener::TrackingControllerDDSListener(Worker &worker, DDSDomainParticipant *part)
: PDWrenchddssender(part, "PDWrench")
  { connectWorker(worker); }

void TrackingControllerDDSListener::DataObjectEmitted(boost::shared_ptr<DataObject> dobj)
{
	// Cast the data object into its real type
	TrackingControllerInfo *localwaypointdriverinfo = dynamic_cast<TrackingControllerInfo *>(dobj.get());
	if (!localwaypointdriverinfo)
		return;

	PDWrenchMessage *msg = PDWrenchMessageTypeSupport::create_data();

	msg->linear[0] = localwaypointdriverinfo->Wrench[0];
	msg->linear[1] = localwaypointdriverinfo->Wrench[1];
	msg->linear[2] = localwaypointdriverinfo->Wrench[2];
	msg->moment[0] = localwaypointdriverinfo->Wrench[3];
	msg->moment[1] = localwaypointdriverinfo->Wrench[4];
	msg->moment[2] = localwaypointdriverinfo->Wrench[5];

	PDWrenchddssender.Send(*msg);
	PDWrenchMessageTypeSupport::delete_data(msg);
}

