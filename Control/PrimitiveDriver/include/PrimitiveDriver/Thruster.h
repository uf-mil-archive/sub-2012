#ifndef PRIMITIVEDRIVER_THRUSTER_H
#define PRIMITIVEDRIVER_THRUSTER_H

#include "HAL/HAL.h"
#include "DataObjects/MotorDriver/MotorDriverInfo.h"
#include "LibSub/Worker/WorkerStateUpdater.h"
#include "LibSub/Worker/WorkerEndpoint.h"

namespace subjugator {
	class Thruster : public WorkerStateUpdater {
		public:
			typedef boost::function<void (const WorkerState &state)> StateChangeCallback;

			Thruster(HAL &hal, int address, int srcaddress, const StateChangeCallback &callback);

			boost::optional<MotorDriverInfo> getInfo() const;
			void setEffort(double effort);

			virtual void updateState(double dt);
			virtual const WorkerState &getWorkerState() const { return endpoint.getWorkerState(); }

		private:
			int address;
			WorkerEndpoint endpoint;
			StateChangeCallback callback;

			void endpointInitCallback();
	};
}

#endif

