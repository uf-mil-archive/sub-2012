#include "VisionDDSCommander.h"
#include "SubMain/Workers/SubWorker.h"
#include "DataObjects/Vision/VisionSetIDs.h"

using namespace subjugator;
using namespace std;

VisionDDSCommander::VisionDDSCommander(Worker &worker, DDSDomainParticipant *participant)
: receiver(participant, "VisionSetIDs", bind(&VisionDDSCommander::receivedVisionSetIDs, this, _1)) {
	visionsetidscmdtoken = worker.ConnectToCommand(0, 5); // can't reference Vision project from DDSCommanders, it needs OpenCV. Next year we're not spreading things out like this
}

void VisionDDSCommander::receivedVisionSetIDs(const VisionSetIDsMessage &msg) {
	vector<int> ids(msg.visionids.length());
	
	for (int i=0;i<msg.visionids.length(); i++) {
		ids[i] = msg.visionids[i];
	}

	boost::shared_ptr<InputToken> ptr = visionsetidscmdtoken.lock();
	if (ptr)
		ptr->Operate(VisionSetIDs(msg.cameraid, ids));
}


