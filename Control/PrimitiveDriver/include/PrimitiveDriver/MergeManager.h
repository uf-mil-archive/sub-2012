#ifndef SUBMAIN_WORKERS_MERGEMANAGER_H
#define SUBMAIN_WORKERS_MERGEMANAGER_H

#include "PrimitiveDriver/DataObjects/MergeInfo.h"
#include "LibSub/State/StateUpdater.h"
#include "LibSub/Worker/WorkerEndpoint.h"
#include "HAL/HAL.h"
#include <boost/function.hpp>

namespace subjugator {
	class MergeManager : public StateUpdaterContainer {
		public:
			typedef boost::function<void (bool)> EStopUpdateCallback;
		
			MergeManager(HAL &hal, const std::string &mergeendpointconf, const std::string &actuatorendpointconf, const EStopUpdateCallback &estopupdatecallback);

			const MergeInfo &getMergeInfo() const { return *mergeendpoint.getDataObject<MergeInfo>(); }
			void setActuators(int flags);

		private:
			EStopUpdateCallback estopupdatecallback;
			WorkerEndpoint mergeendpoint;
			WorkerEndpoint actuatorendpoint;

			bool prevestop;

			void mergeInitCallback();
			void mergeReceiveCallback(const boost::shared_ptr<DataObject> &data);
			void actuatorInitCallback();
	};
}

#endif

