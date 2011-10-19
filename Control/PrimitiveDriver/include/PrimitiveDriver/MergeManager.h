#ifndef SUBMAIN_WORKERS_MERGEMANAGER_H
#define SUBMAIN_WORKERS_MERGEMANAGER_H

#include "HAL/format/DataObjectEndpoint.h"
#include "HAL/SubHAL.h"
#include "DataObjects/Merge/MergeInfo.h"
#include "LibSub/Worker/WorkerStateUpdater.h"
#include "LibSub/Worker/WorkerEndpoint.h"
#include <boost/scoped_ptr.hpp>
#include <cassert>

namespace subjugator {
	class MergeManager : public WorkerStateUpdaterContainer {
		public:
			MergeManager(SubHAL &hal);

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

