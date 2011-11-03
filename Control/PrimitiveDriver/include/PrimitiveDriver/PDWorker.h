#ifndef PRIMITIVEDRIVER_PDWORKER_H
#define PRIMITIVEDRIVER_PDWORKER_H

#include "LibSub/Worker/Worker.h"
#include "LibSub/Worker/WorkerMailbox.h"
#include "LibSub/Worker/WorkerSignal.h"
#include "LibSub/Worker/WorkerEndpoint.h"
#include "DataObjects/PD/PDInfo.h"
#include "PrimitiveDriver/ThrusterManager.h"
#include "PrimitiveDriver/ThrusterMapper.h"
#include "PrimitiveDriver/MergeManager.h"
#include "HAL/SubHAL.h"
#include <Eigen/Dense>
#include <map>

namespace subjugator {
	class PDWorker : public Worker {
		public:
			typedef Eigen::Matrix<double, 6, 1> Vector6d;

			PDWorker(boost::asio::io_service &ioservice);
			virtual const Properties &getProperties() const;

			WorkerMailbox<Vector6d> wrenchmailbox;
			WorkerMailbox<int> actuatormailbox;

			WorkerSignal<std::vector<double> > currentsignal;
			WorkerSignal<PDInfo> infosignal;

		protected:
			virtual void work(double dt);

		private:
			void wrenchSet(const boost::optional<Vector6d> &wrench);
			void actuatorSet(const boost::optional<int> &actuators);
			void thrusterStateChanged(int num, const WorkerState &state);

			virtual void leaveActive();

			SubHAL hal;
			WorkerEndpoint heartbeatendpoint;
			ThrusterManager thrustermanager;
			ThrusterMapper thrustermapper;
			MergeManager mergemanager;

			typedef std::map<int, ThrusterMapper::Entry> ThrusterEntryMap;
			ThrusterEntryMap thrusterentries;
	};
}

#endif // _SubPDWorker_H__

