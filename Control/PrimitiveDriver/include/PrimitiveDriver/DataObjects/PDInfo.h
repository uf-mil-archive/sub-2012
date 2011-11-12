#ifndef DATAOBJECTS_PDINFO_H
#define DATAOBJECTS_PDINFO_H

#include "PrimitiveDriver/DataObjects/MergeInfo.h"
#include "HAL/format/DataObject.h"
#include <boost/array.hpp>
#include <boost/cstdint.hpp>
#include <cassert>

namespace subjugator {
	class PDInfo : public DataObject {
		public:
			PDInfo(int state, boost::int64_t timestamp, const std::vector<double> &currents, const MergeInfo &merge) : state(state),timestamp(timestamp), currents(currents), merge(merge) { assert(currents.size() == 8); }

			inline boost::int64_t getTimestamp() const { return timestamp; }
			inline const std::vector<double> &getCurrents() const { return currents; }
			double getCurrent(int num) const { return currents[num]; }
			inline int getState(){ return state; }
			inline const MergeInfo &getMergeInfo() const { return merge; }

		private:
			int state;
			boost::int64_t timestamp;
			std::vector<double> currents;
			MergeInfo merge;
	};
}

#endif // DATAOBJECTS_PDINFO_H

