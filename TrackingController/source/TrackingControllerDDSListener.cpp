#include "TrackingController/TrackingControllerDDSListener.h"

using namespace subjugator;

TrackingControllerDDSListener::TrackingControllerDDSListener(Worker &worker, DDSDomainParticipant *part)
: PDWrenchddssender(part, "PDWrench"), logddssender(part, "TrackingControllerLog")
  { connectWorker(worker); }

void TrackingControllerDDSListener::DataObjectEmitted(boost::shared_ptr<DataObject> dobj)
{
	// Cast the data object into its real type
	TrackingControllerInfo *trackingcontrollerinfo = dynamic_cast<TrackingControllerInfo *>(dobj.get());
	if (!trackingcontrollerinfo)
		return;

	PDWrenchMessage *msg = PDWrenchMessageTypeSupport::create_data();

	msg->linear[0] = trackingcontrollerinfo->Wrench[0];
	msg->linear[1] = trackingcontrollerinfo->Wrench[1];
	msg->linear[2] = trackingcontrollerinfo->Wrench[2];
	msg->moment[0] = trackingcontrollerinfo->Wrench[3];
	msg->moment[1] = trackingcontrollerinfo->Wrench[4];
	msg->moment[2] = trackingcontrollerinfo->Wrench[5];

	PDWrenchddssender.Send(*msg);
	PDWrenchMessageTypeSupport::delete_data(msg);

	TrackingControllerLogMessage *logmessage = TrackingControllerLogMessageTypeSupport::create_data();

	for (int i=0;i<6;i++) {
		logmessage->control[i] = trackingcontrollerinfo->Wrench[i];
		logmessage->pd_control[i] = trackingcontrollerinfo->pd_control[i];
		logmessage->rise_control[i] = trackingcontrollerinfo->rise_control[i];
		logmessage->nn_control[i] = trackingcontrollerinfo->nn_control[i];
	}

	for (int i=0;i<19;i++)
		for (int j=0;j<5;j++)
			logmessage->V_hat[i][j] = trackingcontrollerinfo->V_hat(i,j);

	for (int i=0;i<6;i++)
		for (int j=0;j<6;j++)
			logmessage->W_hat[i][j] = trackingcontrollerinfo->W_hat(i,j);

	logddssender.Send(*logmessage);
	TrackingControllerLogMessageTypeSupport::delete_data(logmessage);
}

