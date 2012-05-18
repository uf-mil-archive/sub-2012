#include "TrackingController/TrackingControllerWorker.h"
#include "TrackingController/Messages/ControllerGainsMessageSupport.h"
#include "TrackingController/Messages/TrackingControllerLogMessageSupport.h"
#include "TrackingController/Messages/TrajectoryMessageSupport.h"
#include "DDSMessages/LPOSVSSMessageSupport.h"
#include "PrimitiveDriver/Messages/PDWrenchMessageSupport.h"
#include "LibSub/Worker/DDSBuilder.h"
#include "LibSub/Worker/WorkerBuilder.h"
#include <boost/asio.hpp>

using namespace std;
using namespace boost::asio;
using namespace subjugator;

DECLARE_MESSAGE_TRAITS(ControllerGainsMessage);
DECLARE_MESSAGE_TRAITS(LPOSVSSMessage);
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
	dds.killMonitor(worker.killmon);

	dds.receiver(worker.lposvssmailbox, dds.topic<LPOSVSSMessage>("LPOSVSS", TopicQOS::LEGACY));
	dds.receiver(worker.trajectorymailbox, dds.topic<TrajectoryMessage>("Trajectory", TopicQOS::LEGACY));
	dds.receiver(worker.gainsmailbox, dds.topic<ControllerGainsMessage>("ControllerGains", TopicQOS::PERSISTENT));

	dds.sender(worker.wrenchsignal, dds.topic<PDWrenchMessage>("PDWrench", TopicQOS::LEGACY));
	dds.sender(worker.logsignal, dds.topic<TrackingControllerLogMessage>("TrackingControllerLog", TopicQOS::LEGACY));
	dds.sender(worker.gainssignal, dds.topic<ControllerGainsMessage>("ControllerGains", TopicQOS::PERSISTENT));
	dds.sender(worker.initialpointsignal, dds.topic<TrajectoryMessage>("Trajectory", TopicQOS::LEGACY));

	// Start the worker
	builder.runWorker();
}

namespace subjugator {
	template <>
	void from_dds(TrackingControllerWorker::LPOSVSSInfo &lpos, const LPOSVSSMessage &msg) {
		from_dds(lpos.position_ned, msg.position_NED);
		from_dds(lpos.quaternion_ned_b, msg.quaternion_NED_B);
		from_dds(lpos.velocity_ned, msg.velocity_NED);
		from_dds(lpos.angularrate_body, msg.angularRate_BODY);
	}

	template <>
	void from_dds(TrackingController::TrajectoryPoint &tp, const TrajectoryMessage &msg) {
		from_dds(tp.xd, msg.xd);
		from_dds(tp.xd_dot, msg.xd_dot);
	}

	template <>
	void to_dds(TrajectoryMessage &msg, const TrackingController::TrajectoryPoint &tp) {
		to_dds(msg.xd, tp.xd);
		to_dds(msg.xd_dot, tp.xd_dot);
	}

	template <>
	void from_dds(TrackingController::Gains &gains, const ControllerGainsMessage &msg) {
		from_dds(gains.k, msg.k);
		from_dds(gains.ks, msg.ks);
		from_dds(gains.alpha, msg.alpha);
		from_dds(gains.beta, msg.beta);
	}

	template <>
	void to_dds(ControllerGainsMessage &msg, const TrackingController::Gains &gains) {
		to_dds(msg.k, gains.k);
		to_dds(msg.ks, gains.ks);
		to_dds(msg.alpha, gains.alpha);
		to_dds(msg.beta, gains.beta);
	}

	template <>
	void to_dds(PDWrenchMessage &msg, const Vector6d &vec) {
		for (int i=0; i<3; i++)
			msg.linear[i] = vec(i);
		for (int i=0; i<3; i++)
			msg.moment[i] = vec(i+3);
	}

	template <>
	void to_dds(TrackingControllerLogMessage &logmessage, const TrackingControllerWorker::LogData &data) {
		for (int i=0;i<6;i++) {
			logmessage.control[i] = data.out.control[i];
			logmessage.pd_control[i] = data.out.control_pd[i];
			logmessage.rise_control[i] = data.out.control_rise[i];
			logmessage.nn_control[i] = data.out.control_nn[i];
		}

		for (int i=0;i<19;i++)
			for (int j=0;j<5;j++)
				logmessage.V_hat[i][j] = data.v_hat(i,j);

		for (int i=0;i<6;i++)
			for (int j=0;j<6;j++)
				logmessage.W_hat[i][j] = data.w_hat(i,j);
	}
}
