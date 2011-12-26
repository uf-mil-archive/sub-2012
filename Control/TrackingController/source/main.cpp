#include <boost/asio.hpp>

#include "DDSMessages/LPOSVSSMessageSupport.h"
#include "DDSMessages/PDStatusMessageSupport.h"
#include "DDSMessages/PDWrenchMessageSupport.h"
#include "LibSub/DDS/DDSBuilder.h"
#include "LibSub/Worker/WorkerBuilder.h"

#include "TrackingController/Messages/ControllerGainsMessageSupport.h"
#include "TrackingController/Messages/TrackingControllerLogMessageSupport.h"
#include "TrackingController/Messages/TrajectoryMessageSupport.h"
#include "TrackingController/TrackingControllerWorker.h"

using namespace std;
using namespace boost::asio;
using namespace subjugator;

DECLARE_MESSAGE_TRAITS(ControllerGainsMessage);
DECLARE_MESSAGE_TRAITS(LPOSVSSMessage);
DECLARE_MESSAGE_TRAITS(PDStatusMessage);
DECLARE_MESSAGE_TRAITS(PDWrenchMessage);
DECLARE_MESSAGE_TRAITS(TrackingControllerLogMessage);
DECLARE_MESSAGE_TRAITS(TrajectoryMessage);

int main(int argc, char **argv) {
	io_service io;

	// Parse options
	WorkerBuilderOptions options("TrackingController");
	if (!options.parse(argc, argv))
		return 1;

	// Build the worker from the options
	WorkerBuilder<TrackingControllerWorker, DefaultWorkerConstructionPolicy> builder(options, io);
	TrackingControllerWorker &worker = builder.getWorker();

	// Get DDS up
	DDSBuilder dds(io);
	dds.worker(worker);

	dds.receiver(worker.lposvssmailbox, dds.topic<LPOSVSSMessage>("LPOSVSS", TopicQOS::LEGACY));
	dds.receiver(worker.hardwarekilledmailbox, dds.topic<PDStatusMessage>("PDStatus", TopicQOS::LEGACY));
	dds.receiver(worker.trajectorymailbox, dds.topic<TrajectoryMessage>("Trajectory", TopicQOS::LEGACY));
	dds.receiver(worker.gainsmailbox, dds.topic<ControllerGainsMessage>("ControllerGains", TopicQOS::LEGACY));

	dds.sender(worker.wrenchsignal, dds.topic<PDWrenchMessage>("PDWrench", TopicQOS::LEGACY));
	dds.sender(worker.infosignal, dds.topic<TrackingControllerLogMessage>("TrackingControllerLog", TopicQOS::LEGACY));

	// Start the worker
	builder.runWorker();
}

namespace subjugator {
	template <>
	void from_dds(LPOSVSSInfo &res, const LPOSVSSMessage &msg) {
		res = LPOSVSSInfo(msg.state, msg.timestamp, Vector3d(msg.position_NED), Vector4d(msg.quaternion_NED_B), Vector3d(msg.velocity_NED), Vector3d(msg.angularRate_BODY), Vector3d(msg.acceleration_BODY));
	}

	template <>
	void from_dds(bool &hardwareKilled, const PDStatusMessage &pdstatus) {
		hardwareKilled = (pdstatus.flags & (1 << 2)) != 0;
	}

	template <>
	void from_dds(TrajectoryInfo &res, const TrajectoryMessage &msg) {
		res = TrajectoryInfo(msg.timestamp, TrajectoryInfo::Vector6d(msg.xd), TrajectoryInfo::Vector6d(msg.xd_dot));
	}

	template <>
	void from_dds(TrackingControllerGains &new_gains, const ControllerGainsMessage &gains)
	{
		for (int i=0; i<6; i++)
			new_gains.k(i) = gains.k[i];
		for (int i=0; i<6; i++)
			new_gains.ks(i) = gains.ks[i];
		for (int i=0; i<6; i++)
			new_gains.alpha(i) = gains.alpha[i];
		for (int i=0; i<6; i++)
			new_gains.beta(i) = gains.beta[i];
	}

	template <>
	void to_dds(PDWrenchMessage &msg, const Vector6d &vec) {
		for (int i=0; i<3; i++)
			msg.linear[i] = vec(i);
		for (int i=0; i<3; i++)
			msg.moment[i] = vec(i+3);
	}

	template <>
	void to_dds(TrackingControllerLogMessage &logmessage, const TrackingControllerInfo &info) {
		for (int i=0;i<6;i++) {
			logmessage.control[i] = info.Wrench[i];
			logmessage.pd_control[i] = info.pd_control[i];
			logmessage.rise_control[i] = info.rise_control[i];
			logmessage.nn_control[i] = info.nn_control[i];
		}

		for (int i=0;i<19;i++)
			for (int j=0;j<5;j++)
				logmessage.V_hat[i][j] = info.V_hat(i,j);

		for (int i=0;i<6;i++)
			for (int j=0;j<6;j++)
				logmessage.W_hat[i][j] = info.W_hat(i,j);
	}
}
