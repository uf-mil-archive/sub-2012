#ifndef BEHAVIORINFO_H_
#define BEHAVIORINFO_H_


#include "HAL/format/DataObject.h"
#include "DataObjects/Waypoint/Waypoint.h"

#include <string>

namespace subjugator
{
	class BehaviorInfo : public DataObject
	{
	public:
		BehaviorInfo(): behaviorName("None"), miniBehavior("None"), currentObjectID(0), currentWaypoint() {}
		std::string behaviorName;
		std::string miniBehavior;
		int currentObjectID;
		Waypoint currentWaypoint;
	};
}


#endif /* BEHAVIORINFO_H_ */
