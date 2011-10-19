#include "PrimitiveDriver/PDDDSListener.h"
#include "DataObjects/PD/PDInfo.h"
#include <boost/bind.hpp>

using namespace subjugator;
using namespace boost;

PDDDSListener::PDDDSListener(PDWorker &pdworker, DDSDomainParticipant *part)
: ddssender(part, "PDStatus"),
  pdconn(pdworker.infosignal.connect(bind(&PDDDSListener::pdInfoCallback, this, _1))) { }

void PDDDSListener::pdInfoCallback(const PDInfo &info) {
	PDStatusMessage *msg = PDStatusMessageTypeSupport::create_data();
	msg->timestamp = info.getTimestamp();

	for (int i=0; i<8; i++)
		msg->current[i] = info.getCurrent(i);

	const MergeInfo &mergeinfo = info.getMergeInfo();
	msg->estop = mergeinfo.getESTOP();
	msg->flags = mergeinfo.getFlags();
	msg->tickcount = mergeinfo.getTickCount();
	msg->voltage16 = mergeinfo.getRail16Voltage();
	msg->current16 = mergeinfo.getRail16Current();
	msg->voltage32 = mergeinfo.getRail32Voltage();
	msg->current32 = mergeinfo.getRail32Current();

	ddssender.Send(*msg);
	PDStatusMessageTypeSupport::delete_data(msg);
}

