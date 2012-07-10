#include "SubControl/Stats.h"
#include "LibSub/Math/Quaternion.h"

using namespace subjugator;
using namespace boost;
using namespace Eigen;
using namespace std;

Stats::Stats(Participant &part) :
	lposvsstopic(part, "LPOSVSS"),
	lposvssreceiver(lposvsstopic),
	efforttopic(part, "PDEffort", TopicQOS::RELIABLE),
	effortreceiver(efforttopic),
	statustopic(part, "PDStatus"),
	statusreceiver(statustopic),
	hydrophonetopic(part, "Hydrophone"),
	hydrophonereceiver(hydrophonetopic)
{ }

Stats::Data Stats::getData() {
	Data data;

	shared_ptr<PDStatusMessage> pdstatus = statusreceiver.read();
	if (pdstatus) {
		data.rails.avail = true;
		data.rails.r16.voltage = pdstatus->voltage16;
		data.rails.r16.current = pdstatus->current16;
		data.rails.r32.voltage = pdstatus->voltage32;
		data.rails.r32.current = pdstatus->current32;
	} else {
		data.rails.avail = false;
	}

	shared_ptr<PDEffortMessage> pdefforts = effortreceiver.read();
	if (pdefforts) {
		data.efforts.avail = true;
		data.efforts.lfor = pdefforts->efforts[4]; // ugly, need to re-do PD messages so thruster map can be obtained over DDS
		data.efforts.rfor = pdefforts->efforts[5];
		data.efforts.fs = pdefforts->efforts[2];
		data.efforts.rs = pdefforts->efforts[6];
		data.efforts.flv = pdefforts->efforts[1];
		data.efforts.frv = pdefforts->efforts[0];
		data.efforts.rlv = pdefforts->efforts[3];
		data.efforts.rrv = pdefforts->efforts[7];
	} else {
		data.efforts.avail = false;
	}

	shared_ptr<LPOSVSSMessage> lposvss = lposvssreceiver.read();
	if (lposvss) {
		data.lposvss.avail = true;
		Vector3d rpy = MILQuaternionOps::Quat2Euler(Vector4d(lposvss->quaternion_NED_B));
		data.lposvss.x = lposvss->position_NED[0];
		data.lposvss.y = lposvss->position_NED[1];
		data.lposvss.z = lposvss->position_NED[2];
		data.lposvss.R = rpy[0];
		data.lposvss.P = rpy[1];
		data.lposvss.Y = rpy[2];
	} else {
		data.lposvss.avail = false;
	}

	shared_ptr<HydrophoneMessage> hydrophone = hydrophonereceiver.read();
	if (hydrophone) {
		data.hydrophone.avail = true;
		data.hydrophone.heading = hydrophone->heading;
		data.hydrophone.declination = hydrophone->declination;
		data.hydrophone.dist = hydrophone->distance;
		data.hydrophone.freq = hydrophone->frequency;
	} else {
		data.hydrophone.avail = false;
	}

	return data;
}
