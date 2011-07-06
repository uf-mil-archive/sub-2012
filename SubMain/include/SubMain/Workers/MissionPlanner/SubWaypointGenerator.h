#ifndef SUBWAYPOINTGENERATOR_H
#define SUBWAYPOINTGENERATOR_H

#include "SubMain/SubPrerequisites.h"
#include "SubMain/Workers/MissionPlanner/SubMissionCamera.h"
#include "SubMain/Workers/LPOSVSS/SubTriad.h"

#include "DataObjects/LPOSVSS/LPOSVSSInfo.h"
#include "DataObjects/Vision/FinderResult2D.h"
#include "DataObjects/Vision/FinderResult3D.h"
#include "DataObjects/Waypoint/Waypoint.h"

#include <cmath>

namespace subjugator
{
	class WaypointGenerator
	{
	public:
		WaypointGenerator(std::vector<MissionCamera>& cams);

		boost::shared_ptr<Waypoint> GenerateFrom2D(const LPOSVSSInfo& lposInfo, const FinderResult2D& object2d, const Vector2d& k, double hoverDistance, bool servo);
		boost::shared_ptr<Waypoint> GenerateFrom3D(const LPOSVSSInfo& lposInfo, const FinderResult3D& object3d, double hoverDistance);

	private:
		std::vector<MissionCamera> cameras;
	};
}


#endif /* SUBWAYPOINTGENERATOR_H */
