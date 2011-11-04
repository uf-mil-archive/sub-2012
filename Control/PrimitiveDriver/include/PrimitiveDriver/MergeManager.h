#ifndef SUBMAIN_WORKERS_MERGEMANAGER_H
#define SUBMAIN_WORKERS_MERGEMANAGER_H

#include "LibSub/State/StateUpdater.h"
#include "LibSub/Worker/WorkerEndpoint.h"
#include "HAL/HAL.h"
#include "DataObjects/Merge/MergeInfo.h"

namespace subjugator {
	class MergeManager : public StateUpdaterContainer {
		public:
			MergeManager(HAL &hal);

			MergeInfo getMergeInfo() const;
			void setActuators(int flags);

		private:
			WorkerEndpoint mergeendpoint;
			WorkerEndpoint actuatorendpoint;

			void mergeInitCallback();
			void actuatorInitCallback();
	};
}

#endif

