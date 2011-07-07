#include "DDSCommanders/MissionPlannerDDSCommander.h"
#include "DataObjects/LPOSVSS/LPOSVSSInfo.h"
#include "DataObjects/PD/PDInfo.h"
#include "DataObjects/Merge/MergeInfo.h"
#include "DataObjects/Vision/FinderResult2DVec.h"
#include "DataObjects/Vision/FinderResult3DVec.h"
#include "SubMain/Workers/MissionPlanner/SubMissionPlannerWorker.h"

using namespace subjugator;
using namespace boost;
using namespace Eigen;

MissionPlannerDDSCommander::MissionPlannerDDSCommander(Worker &worker, DDSDomainParticipant *participant)
: lposvssreceiver(participant, "LPOSVSS", bind(&MissionPlannerDDSCommander::receivedLPOSVSSInfo, this, _1)),
  pdstatusreceiver(participant, "PDStatus", bind(&MissionPlannerDDSCommander::receivedPDStatusInfo, this, _1)),
  finderlistreceiver(participant, "Vision", bind(&MissionPlannerDDSCommander::receivedFinderMessageListResult, this, _1)) {
	lposvsscmdtoken = worker.ConnectToCommand(MissionPlannerWorkerCommands::SetLPOSVSSInfo, 5);
	pdstatuscmdtoken = worker.ConnectToCommand(MissionPlannerWorkerCommands::SetPDInfo, 5);
	vision2dcmdtoken = worker.ConnectToCommand(MissionPlannerWorkerCommands::SetCam2DInfo, 5);
	vision3dcmdtoken = worker.ConnectToCommand(MissionPlannerWorkerCommands::SetCam3DInfo, 5);
}

void MissionPlannerDDSCommander::receivedLPOSVSSInfo(const LPOSVSSMessage &lposvssinfo) {
	int state;
	boost::int64_t timestamp;
	Vector3d position_NED;
	Vector4d quaternion_NED_B;
	Vector3d velocity_NED;
	Vector3d angularRate_BODY;
	Vector3d acceleration_BODY;

	state = lposvssinfo.state;
	timestamp = lposvssinfo.timestamp;

	for (int i=0; i<3; i++)
		position_NED(i) = lposvssinfo.position_NED[i];
	for (int i=0; i<4; i++)
		quaternion_NED_B(i) = lposvssinfo.quaternion_NED_B[i];
	for (int i=0; i<3; i++)
		velocity_NED(i) = lposvssinfo.velocity_NED[i];
	for (int i=0; i<3; i++)
		angularRate_BODY(i) = lposvssinfo.angularRate_BODY[i];
	for (int i=0; i<3; i++)
		acceleration_BODY(i) = lposvssinfo.acceleration_BODY[i];

	shared_ptr<InputToken> ptr = lposvsscmdtoken.lock();
	if (ptr)
	{
		ptr->Operate(LPOSVSSInfo(state, timestamp, position_NED, quaternion_NED_B, velocity_NED, angularRate_BODY, acceleration_BODY));
	}
}

void MissionPlannerDDSCommander::receivedPDStatusInfo(const PDStatusMessage &pdstatusinfo) {
	unsigned long long timestamp;
	short state;
	std::vector<double> current(8);
	bool estop;

	unsigned long tickcount;
	unsigned long flags;

	double current16;
	double voltage16;
	double current32;
	double voltage32;

	state = pdstatusinfo.state;
	timestamp = pdstatusinfo.timestamp;
	tickcount = pdstatusinfo.tickcount;
	flags = pdstatusinfo.flags;

	for (int i=0; i<8; i++)
		current[i] = pdstatusinfo.current[i];

	estop = pdstatusinfo.estop;

	current16 = pdstatusinfo.current16;
	voltage16 = pdstatusinfo.voltage16;
	current32 = pdstatusinfo.current32;
	voltage32 = pdstatusinfo.voltage32;

	shared_ptr<InputToken> ptr = pdstatuscmdtoken.lock();
	if (ptr)
	{
		ptr->Operate(PDInfo(state, timestamp, current, MergeInfo(timestamp, tickcount, flags, current16, voltage16, current32, voltage32)));
	}
}

void MissionPlannerDDSCommander::receivedFinderMessageListResult(const FinderMessageList &findermessages) {
	shared_ptr<InputToken> ptr = vision2dcmdtoken.lock();
	
	FinderResult2DVec vec2d;
	for (int i=0;i<findermessages.messages2d.length(); i++) {
		const Finder2DMessage &msg = findermessages.messages2d[i];
		vec2d.vec.push_back(FinderResult2D(findermessages.cameraid, msg.objectid, msg.u, msg.v, msg.scale, msg.angle));
	}
	ptr->Operate(vec2d);
	
	ptr = vision3dcmdtoken.lock();
	FinderResult3DVec vec3d;
	for (int i=0;i<findermessages.messages3d.length(); i++) {
		const Finder3DMessage &msg = findermessages.messages3d[i];
		vec3d.vec.push_back(FinderResult3D(findermessages.cameraid, msg.objectid, msg.x, msg.y, msg.z, msg.ang1, msg.ang2, msg.ang3));
	}
	ptr->Operate(vec3d);
}


