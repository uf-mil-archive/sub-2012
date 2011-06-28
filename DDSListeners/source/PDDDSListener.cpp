#include "DDSListeners/PDDDSListener.h"
#include "DataObjects/PD/PDStatus.h"

using namespace subjugator;

PDDDSListener::PDDDSListener(Worker &worker, DDSDomainParticipant *part)
: Listener(worker), ddssender(part, "PDStatus") { }

void PDDDSListener::DataObjectEmitted(boost::shared_ptr<DataObject> dobj)
{
	// Cast the data object into its real type
	PDStatus *status = dynamic_cast<PDStatus *>(dobj.get());
	if(!status)
		return;

	PDStatusMessage *msg = PDStatusMessageTypeSupport::create_data();
	msg->timestamp = status->getTimestamp();

	for(int i=0; i<8; i++)
		msg->current[i] = status->getCurrent(i);
	msg->kill = status->getKill();

	ddssender.Send(*msg);
	PDStatusMessageTypeSupport::delete_data(msg);
}

