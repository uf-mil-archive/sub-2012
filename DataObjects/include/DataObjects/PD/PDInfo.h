#ifndef DATAOBJECTS_PDINFO_H
#define DATAOBJECTS_PDINFO_H

#include "HAL/format/DataObject.h"
#include <boost/array.hpp>
#include <boost/cstdint.hpp>
#include <cassert>

namespace subjugator {
	class PDInfo : public DataObject {
		public:
			PDInfo(int state, boost::int64_t timestamp, const std::vector<double> &currents, bool estop) : state(state),timestamp(timestamp), currents(currents), estop(estop) { assert(currents.size() == 8); }

			inline boost::int64_t getTimestamp() const { return timestamp; }
			inline const std::vector<double> &getCurrents() const { return currents; }
			double getCurrent(int num) const { return currents[num]; }
			inline bool getESTOP() const { return estop; }
			inline int getState(){ return state; }

		private:
			int state;
			boost::int64_t timestamp;
			std::vector<double> currents;
			bool estop;
	};
}

#endif // DATAOBJECTS_PDINFO_H

