#ifndef WAYPOINT_H
#define WAYPOINT_H

#include "HAL/format/DataObject.h"
#include <Eigen/Dense>

using namespace Eigen;

namespace subjugator {
	class Waypoint : public DataObject {
		public:
			Waypoint();
			Waypoint(const Vector3d& position, const Vector3d& rpy)
			{
				Position_NED = position;
				RPY = rpy;
			}

			Vector3d Position_NED;
			Vector3d RPY;

			Vector3d getPosition_NED() { return Position_NED; };
			void setPosition_NED(Vector3d t) { Position_NED = t; };
			Vector3d getRPY() { return RPY; };
			void setRPY(Vector3d t) { RPY = t; };

			double getX() { return Position_NED(0,0); };
			void setX(double value) { Position_NED(0,0) = value; }
			double getY() { return Position_NED(1,0); }
			void setY(double value) { Position_NED(1,0) = value; }
			double getZ() { return Position_NED(2,0); };
			void setZ(double value) { Position_NED(2,0) = value; };

			double getRoll() { return RPY(0,0); };
			void setRoll(double value) { RPY(0,0) = value; };
			double getPitch() { return RPY(1,0); };
			void setPitch(double value) { RPY(1,0) = value; };
			double getYaw() { return RPY(2,0); };
			void setYaw(double value) { RPY(2,0) = value; };
	};
}

#endif /* WAYPOINT_H */
