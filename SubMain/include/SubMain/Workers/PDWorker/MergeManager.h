#ifndef SUBMAIN_WORKERS_MERGEMANAGER_H
#define SUBMAIN_WORKERS_MERGEMANAGER_H

#include "HAL/format/DataObjectEndpoint.h"
#include "HAL/SubHAL.h"
#include "DataObjects/Merge/MergeInfo.h"
#include <boost/scoped_ptr.hpp>

namespace subjugator {
	class MergeManager {
		public:
			MergeManager(SubHAL &hal);

			inline const MergeInfo &getMergeInfo() const { return info; }
			
			void setActuators(int flags);

		private:
			boost::scoped_ptr<DataObjectEndpoint> endpoint;

			MergeInfo info;

			void halReceiveCallback(std::auto_ptr<DataObject> &dobj);
			void halStateChangeCallback();
	};
}

#endif

