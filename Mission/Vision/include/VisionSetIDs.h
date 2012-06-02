#ifndef DATAOBJECTS_VISION_H
#define DATAOBJECTS_VISION_H

#include <vector>

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

