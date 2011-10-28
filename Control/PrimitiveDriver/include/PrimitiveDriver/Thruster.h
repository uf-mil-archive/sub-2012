#ifndef _SubThruster_H__
#define _SubThruster_H__

#include "SubMain/SubPrerequisites.h"
#include <Eigen/Dense>
#include "HAL/format/DataObjectEndpoint.h"
#include "DataObjects/MotorDriver/MotorDriverInfo.h"
#include "DataObjects/MotorDriver/MotorDriverDataObjectFormatter.h"
#include "DataObjects/MotorDriver/SetReference.h"
#include "HAL/format/Sub7EPacketFormatter.h"
#include "HAL/HAL.h"
#include "LibSub/Worker/WorkerStateUpdater.h"
#include "LibSub/Worker/WorkerEndpoint.h"
#include <boost/scoped_ptr.hpp>

namespace subjugator {
	class Thruster : public WorkerStateUpdater {
		public:
			typedef boost::function<void (const WorkerState &state)> StateChangeCallback;

			Thruster(HAL &hal, int address, int srcaddress, const StateChangeCallback &callback);

			MotorDriverInfo getInfo() const { return *endpoint.getDataObject<MotorDriverInfo>(); }
			void setEffort(double effort);

			virtual void updateState(double dt);
			virtual const WorkerState &getWorkerState() const { return endpoint.getWorkerState(); }

		private:
			WorkerEndpoint endpoint;
			StateChangeCallback callback;

			void endpointInitCallback();
	};
}

#endif

