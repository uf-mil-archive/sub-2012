#ifndef DATAOBJECTS_VISION_H
#define DATAOBJECTS_VISION_H

#include <boost/array.hpp>
#include <boost/cstdint.hpp>
#include <cassert>

namespace subjugator {
	class VisionSetIDs {
		public:
			VisionSetIDs() { }
			VisionSetIDs(int cameraID, const std::vector<int> &ids) : cameraID(cameraID), ids(ids) { }

			int cameraID;
			std::vector<int> ids;
	};
}

#endif // DATAOBJECTS_PDINFO_H

