#ifndef DATAOBJECTS_PDSTATUS_H
#define DATAOBJECTS_PDSTATUS_H

#include "HAL/format/DataObject.h"
#include <boost/array.hpp>
#include <boost/cstdint.hpp>
#include <cassert>

namespace subjugator {
	class PDStatus : public DataObject {
		public:
			PDStatus(const std::vector<double> &currents, bool kill) : currents(currents), kill(kill) { assert(currents.size() == 8); }

			inline boost::int64_t getTimestamp() const { return timestamp; }
			inline const std::vector<double> &getCurrents() const { return currents; }
			double getCurrent(int num) const { return currents[num]; }
			inline bool getKill() const { return kill; }

		private:
			boost::int64_t timestamp;
			std::vector<double> currents;
			bool kill;
	};
}

#endif

