#ifndef _SubPDWorker_H__
#define _SubPDWorker_H__

#include "LibSub/Worker/Worker.h"
#include "LibSub/Worker/WorkerMailbox.h"
#include "LibSub/Worker/WorkerSignal.h"
#include "LibSub/Worker/WorkerEndpoint.h"
#include "DataObjects/PD/PDInfo.h"
#include "PrimitiveDriver/ThrusterManager.h"
#include "PrimitiveDriver/MergeManager.h"
#include "HAL/SubHAL.h"
#include <vector>

namespace subjugator {
	class PDWorker : public Worker {
		public:
			typedef Matrix<double, 6, 1> Vector6d;

			PDWorker(boost::asio::io_service &ioservice);
			virtual const Properties &getProperties() const;

			void setWrench(const Vector6d &wrench);
			void setActuators(int flags);

			WorkerSignal<std::vector<double> > currentsignal;
			WorkerSignal<PDInfo> infosignal;

		protected:
			virtual void work(double dt);

		private:
			SubHAL hal;
			WorkerEndpoint heartbeatendpoint;
			ThrusterManager thrustermanager;
			MergeManager mergemanager;

			virtual void leaveActive();
	};
}

#endif // _SubPDWorker_H__

