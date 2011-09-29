#include "DDSCommanders/EquTrajectoryGeneratorDDSCommander.h"
#include "SubMain/Workers/EquTrajectoryGenerator/EquTrajectoryGeneratorWorker.h"
#include "DataObjects/Waypoint/Waypoint.h"
#include "DataObjects/LPOSVSS/LPOSVSSInfo.h"
#include "DataObjects/PD/PDInfo.h"
#include "DataObjects/Merge/MergeInfo.h"
#include "DataObjects/TrackingController/ControllerGains.h"
#include <boost/bind.hpp>
#include <iostream>

using namespace subjugator;
using namespace boost;

EquTrajectoryGeneratorDDSCommander::EquTrajectoryGeneratorDDSCommander(Worker &worker, DDSDomainParticipant *participant)
: pdstatusreceiver(participant, "PDStatus", bind(&EquTrajectoryGeneratorDDSCommander::receivedPDStatusInfo, this, _1)) {
	pdstatuscmdtoken = worker.ConnectToCommand((int)EquTrajectoryGeneratorWorkerCommands::SetPDInfo, 5);
}

void EquTrajectoryGeneratorDDSCommander::receivedPDStatusInfo(const PDStatusMessage &pdstatusinfo) {
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

