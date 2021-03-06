#ifndef PRIMITIVEDRIVER_THRUSTER_H
#define PRIMITIVEDRIVER_THRUSTER_H

#include "PrimitiveDriver/DataObjects/MotorDriverInfo.h"
#include "HAL/HAL.h"
#include "LibSub/State/StateUpdater.h"
#include "LibSub/Worker/WorkerEndpoint.h"

namespace subjugator {
	class Thruster : public StateUpdater {
		public:
			Thruster(const std::string &name, DataObjectEndpoint *endpoint, int srcaddress);

			boost::shared_ptr<MotorDriverInfo> getInfo() const;
			void setEffort(double effort);

			virtual void updateState(double dt);
			virtual const State &getState() const { return state; }

		private:
			WorkerEndpoint endpoint;

			State state;
			void endpointInitCallback();
	};
}

#endif

