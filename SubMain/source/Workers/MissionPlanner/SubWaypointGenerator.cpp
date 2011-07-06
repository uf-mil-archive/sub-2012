#include "SubMain/Workers/MissionPlanner/SubWaypointGenerator.h"

using namespace std;
using namespace subjugator;

WaypointGenerator::WaypointGenerator(std::vector<MissionCamera>& cams)
{
	cameras = cams; // copy the list into the waypoint generator

	// The triad for the cameras is built from the body x and z vectors
	Triad camTriad(Vector4d(1.0,0.0,0.0,0.0), Vector3d(1.0,0.0,0.0), Vector3d(0.0,0.0,1.0));

	for(size_t i = 0; i < cams.size(); i++)
	{
		camTriad.Update(cameras[i].X, cameras[i].Z);
		cameras[i].Quaternion = camTriad.getQuaternion();
	}
}

std::auto_ptr<Waypoint> WaypointGenerator::GenerateFrom2D(const FinderResult2D& object2d, const Vector2d& k, double hoverDistance, bool servo)
{
	// TODO object enumeration
	if(object2d.objectID == 0 || object2d.scale == 0.0)
		return std::auto_ptr<Waypoint>();

	Vector3d err = Vector3d::Zero();
	double yaw = 0.0;

	if(!servo)
	{
		double distance = 185.0*pow(object2d.scale, -0.59);
		// TODO change to cameraID!
		double xd = distance * (object2d.u - cameras[object2d.cameraID].cc(0)) / cameras[object2d.cameraID].fc(0);
		double yd = distance * (object2d.v - cameras[object2d.cameraID].cc(1)) / cameras[object2d.cameraID].fc(1);

		err(0) = k(0)*xd;
		err(1) = k(1)*yd;
		err(2) = distance;

		yaw = object2d.angle;
	}
	else
	{
		// TODO forward camera is in this check
		if(object2d.cameraID == 0)
		{
			double yawError = object2d.u - cameras[object2d.cameraID].cc(0);
			double yd = object2d.v - cameras[object2d.cameraID].cc(1);

			err(0) = 0.0;
			err(1) = k(1) * yd;
			err(2) = 0.0;

			yaw = k(0)*yawError;
		}
		else
		{
			double xd = object2d.u - cameras[object2d.cameraID].cc(0);
			double yd = object2d.v - cameras[object2d.cameraID].cc(1);

			err(0) = k(0)*xd;
			err(1) = k(1)*yd;
			err(2) = 0.0;

			yaw = object2d.angle;
		}
	}

	// The eventual output is in the NED frame. The intermediate calculations are done in the
	// BODY frame. First rotate the camera into body
	//resWP->Position_NED = MILQuaternionOps::QuatRotate(cameras[object2d.cameraID].Quaternion, err);
	//resWP->RPY = Vector3d(0.0, 0.0, yaw);

	// Calculate the waypoint with the desired hover distance. We need a unit vector from the current
	// position to the desired postion

	return std::auto_ptr<Waypoint>();
}
