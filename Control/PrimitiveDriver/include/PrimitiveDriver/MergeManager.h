#ifndef SUBMAIN_WORKERS_MERGEMANAGER_H
#define SUBMAIN_WORKERS_MERGEMANAGER_H

#include "PrimitiveDriver/DataObjects/MergeInfo.h"
#include "LibSub/State/StateUpdater.h"
#include "LibSub/Worker/WorkerEndpoint.h"
#include "HAL/HAL.h"

namespace subjugator {
	class MergeManager : public StateUpdaterContainer {
		public:
			MergeManager(HAL &hal);

			const MergeInfo &getMergeInfo() const { return *mergeendpoint.getDataObject<MergeInfo>(); }
			void setActuators(int flags);

		private:
			WorkerEndpoint mergeendpoint;
			WorkerEndpoint actuatorendpoint;

			void mergeInitCallback();
			void actuatorInitCallback();
	};
}

#endif

