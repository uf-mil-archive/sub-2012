#include "DDSCommanders/LPOSVSSDDSCommander.h"
#include "SubMain/Workers/LPOSVSS/SubLPOSVSSWorker.h"
#include "DataObjects/PD/PDWrench.h"
#include <boost/bind.hpp>

using namespace subjugator;
using namespace boost;

LPOSVSSDDSCommander::LPOSVSSDDSCommander(Worker &worker, DDSDomainParticipant *participant)
: depthreceiver(participant, "Depth", bind(&LPOSVSSDDSCommander::receivedDepth, this, _1)),
  dvlreceiver(participant, "DVL", bind(&LPOSVSSDDSCommander::receivedDVL, this, _1)),
  imureceiver(participant, "IMU", bind(&LPOSVSSDDSCommander::receivedIMU, this, _1)),
  pdreceiver(participant, "PDStatus", bind(&LPOSVSSDDSCommander::receivedPDInfo, this, _1)) {
	depthtoken = worker.ConnectToCommand(LPOSVSSWorkerCommands::SetDepth, 5);
	dvltoken = worker.ConnectToCommand(LPOSVSSWorkerCommands::SetDVL, 5);
	imutoken = worker.ConnectToCommand(LPOSVSSWorkerCommands::SetIMU, 5);
	pdtoken = worker.ConnectToCommand(LPOSVSSWorkerCommands::SetCurrents, 5);
}

void LPOSVSSDDSCommander::receivedDepth(const DepthMessage &depth) {
	depthtoken.lock()->Operate(DepthInfo(depth.timestamp, 0, 0, depth.depth, depth.thermistertemp, depth.humidity, depth.humiditytemp));
}

void LPOSVSSDDSCommander::receivedDVL(const DVLMessage &dvl) {
	dvltoken.lock()->Operate(DVLHighresBottomTrack(dvl.timestamp, Vector3d(dvl.velocity), dvl.velocityerror, dvl.good));
}

void LPOSVSSDDSCommander::receivedIMU(const IMUMessage &imu) {
	imutoken.lock()->Operate(IMUInfo(imu.flags, imu.supply, imu.temp, imu.timestamp, Vector3d(imu.acceleration), Vector3d(imu.angular_rate), Vector3d(imu.mag_field)));
}

void LPOSVSSDDSCommander::receivedPDInfo(const PDStatusMessage &status) {
	std::vector<double> currents(8);

	for (int i=0;i<8;i++)
		currents[i] = status.current[0];

	pdtoken.lock()->Operate(PDInfo(0, status.timestamp, currents, status.estop));
}

