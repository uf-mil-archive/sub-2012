#ifndef DATAOBJECTS_VISION_H
#define DATAOBJECTS_VISION_H

#include "HAL/format/DataObject.h"
#include <boost/array.hpp>
#include <boost/cstdint.hpp>
#include <cassert>

namespace subjugator {
	class VisionSetIDs : public DataObject {
		public:
			VisionSetIDs(int cameraID, const std::vector<int> &ids) : cameraID(cameraID), ids(ids) { }

			int getCameraID() const { return cameraID; }
			const std::vector<int> &getIDs() const { return ids; }

		private:
			int cameraID;
			std::vector<int> ids;
	};
}

#endif // DATAOBJECTS_PDINFO_H

