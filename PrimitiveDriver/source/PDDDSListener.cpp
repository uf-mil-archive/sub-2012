#include "PrimitiveDriver/PDDDSListener.h"
#include "DataObjects/PD/PDInfo.h"

using namespace subjugator;

PDDDSListener::PDDDSListener(Worker &worker, DDSDomainParticipant *part)
: ddssender(part, "PDStatus") { connectWorker(worker); }

void PDDDSListener::DataObjectEmitted(boost::shared_ptr<DataObject> dobj)
{
	// Cast the data object into its real type
	PDInfo *status = dynamic_cast<PDInfo *>(dobj.get());
	if(!status)
		return;

	PDStatusMessage *msg = PDStatusMessageTypeSupport::create_data();
	msg->timestamp = status->getTimestamp();

	for(int i=0; i<8; i++)
		msg->current[i] = status->getCurrent(i);
	msg->estop = status->getMergeInfo().getESTOP();
	msg->flags = status->getMergeInfo().getFlags();
	msg->tickcount = status->getMergeInfo().getTickCount();
	msg->voltage16 = status->getMergeInfo().getRail16Voltage();
	msg->current16 = status->getMergeInfo().getRail16Current();
	msg->voltage32 = status->getMergeInfo().getRail32Voltage();
	msg->current32 = status->getMergeInfo().getRail32Current();

	ddssender.Send(*msg);
	PDStatusMessageTypeSupport::delete_data(msg);
}

