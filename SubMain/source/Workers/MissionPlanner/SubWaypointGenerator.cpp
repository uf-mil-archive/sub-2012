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

boost::shared_ptr<Waypoint> WaypointGenerator::GenerateFrom2D(const LPOSVSSInfo& lposInfo, const Vector3d& lposRPY, const FinderResult2D& object2d, const Vector2d& k, double hoverDistance, bool servo)
{
	// TODO object enumeration
	if(object2d.objectID == 0 || object2d.scale == 0.0)
		return boost::shared_ptr<Waypoint>();

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
		if(object2d.cameraID == MissionCameraIDs::Front)
		{
			Vector3d camRot = MILQuaternionOps::Quat2Euler(cameras[object2d.cameraID].Quaternion);
			cout << "CamRot:\n" << camRot << endl;

			double yawError = -1*(object2d.v - cameras[object2d.cameraID].cc(1));
			double yd = object2d.u - cameras[object2d.cameraID].cc(0);

			err(0) = k(0) * yd;
			err(1) = 0.0;
			err(2) = 0.0;

			yaw = k(1)*yawError;
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

	boost::shared_ptr<Waypoint> resWP = boost::shared_ptr<Waypoint>(new Waypoint());

	// The eventual output is in the NED frame. The intermediate calculations are done in the
	// BODY frame. First rotate the camera into body
	resWP->Position_NED = MILQuaternionOps::QuatRotate(cameras[object2d.cameraID].Quaternion, err);
	resWP->RPY = Vector3d(0.0, 0.0, AttitudeHelpers::DAngleClamp(yaw + lposRPY(2)));
	resWP->isRelative = false;

	// Calculate the waypoint with the desired hover distance. We need a unit vector from the current
	// position to the desired postion. The output of the camera gains is a relative vector from the sub.
	Vector3d unitdir = resWP->Position_NED;
	unitdir.normalize();
	resWP->Position_NED -= hoverDistance*unitdir;

	resWP->Position_NED = MILQuaternionOps::QuatRotate(lposInfo.getQuat_NED_B(), resWP->Position_NED) + lposInfo.getPosition_NED();

	return resWP;
}

boost::shared_ptr<Waypoint> WaypointGenerator::GenerateFrom3D(const LPOSVSSInfo& lposInfo, const FinderResult3D& object3d, double hoverDistance)
{
	// TODO object enumeration
	if(object3d.objectID == 0 )
		return boost::shared_ptr<Waypoint>();

	Vector3d errPOS(object3d.x, object3d.y, object3d.z-hoverDistance);

	boost::shared_ptr<Waypoint> resWP = boost::shared_ptr<Waypoint>(new Waypoint());

	// The eventual output is in the NED frame. The intermediate calculations are done in the
	// BODY frame. First rotate the camera into body
	resWP->Position_NED = MILQuaternionOps::QuatRotate(cameras[object3d.cameraID].Quaternion, errPOS);
	resWP->RPY = Vector3d(0.0, 0.0, object3d.ang3);
	resWP->isRelative = false;

	return resWP;
}

