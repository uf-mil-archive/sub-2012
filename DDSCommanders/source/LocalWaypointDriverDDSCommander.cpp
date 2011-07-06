#include "DDSCommanders/LocalWaypointDriverDDSCommander.h"
#include "SubMain/Workers/WaypointController/LocalWaypointDriverWorker.h"
#include "DataObjects/Waypoint/Waypoint.h"
#include "DataObjects/LPOSVSS/LPOSVSSInfo.h"
#include "DataObjects/PD/PDInfo.h"
#include "DataObjects/Merge/MergeInfo.h"
#include <boost/bind.hpp>
#include <iostream>

using namespace subjugator;
using namespace boost;

LocalWaypointDriverDDSCommander::LocalWaypointDriverDDSCommander(Worker &worker, DDSDomainParticipant *participant)
: waypointreceiver(participant, "LocalWaypointDriver", bind(&LocalWaypointDriverDDSCommander::receivedWaypoint, this, _1)),
  lposvssreceiver(participant, "LPOSVSS", bind(&LocalWaypointDriverDDSCommander::receivedLPOSVSSInfo, this, _1)),
  pdstatusreceiver(participant, "PDStatus", bind(&LocalWaypointDriverDDSCommander::receivedPDStatusInfo, this, _1)){
	waypointcmdtoken = worker.ConnectToCommand((int)LocalWaypointDriverWorkerCommands::SetWaypoint, 5);
	lposvsscmdtoken = worker.ConnectToCommand((int)LocalWaypointDriverWorkerCommands::SetLPOSVSSInfo, 5);
	pdstatuscmdtoken = worker.ConnectToCommand((int)LocalWaypointDriverWorkerCommands::SetPDInfo, 5);
}

void LocalWaypointDriverDDSCommander::receivedWaypoint(const LocalWaypointDriverMessage &waypoint) {
	Vector3d position_ned;
	Vector3d rpy;

	for (int i=0; i<3; i++)
		position_ned(i) = waypoint.position_ned[i];
	for (int i=0; i<3; i++)
		rpy(i) = waypoint.rpy[i];

	shared_ptr<InputToken> ptr = waypointcmdtoken.lock();
	if (ptr)
	{
		ptr->Operate(Waypoint(position_ned,rpy));
	}
}

void LocalWaypointDriverDDSCommander::receivedLPOSVSSInfo(const LPOSVSSMessage &lposvssinfo) {
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

void LocalWaypointDriverDDSCommander::receivedPDStatusInfo(const PDStatusMessage &pdstatusinfo) {
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


