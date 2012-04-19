#ifndef DATAOBJECTS_PDINFO_H
#define DATAOBJECTS_PDINFO_H

#include <boost/array.hpp>
#include <boost/cstdint.hpp>
#include <cassert>

namespace subjugator {
	struct PDInfo {
		std::vector<double> currents;
	};
}

#endif // DATAOBJECTS_PDINFO_H

