#include "SuckySim/SimSub.h"

using namespace subjugator;
using namespace Eigen;
using namespace std;

SimSub::SimSub(std::string name, int objectID, QColor color)
	: SimObject(name, objectID, color), length(.6223),
	  width(0.1778), podLength(0.5163), podWidth(0.115),
	  velocity_NED(0.0,0.0,0.0)
{
	Matrix3d fcamCal;
	fcamCal << 720,0,0,
			   0,720,0,
			   320,240,1;
	MissionCamera fCam(MissionCameraIDs::Front,
			Vector3d(0.0,0.0,1.0),	// X vector
			Vector3d(0.0,-1.0,0.0),	// Y vector
			Vector3d(1.0,0.0,0.0),	// Z vector
			Vector4d(1.0,0.0,0.0,0.0),	// Quat - populated later by waypoint generator
			Vector2d(319.54324, 208.29877),		// cc
			Vector2d(967.16810, 965.86543),		//fc
			fcamCal);

	forwardCamera = std::auto_ptr<SimCamera>(new SimCamera(fCam, 37.0, 640 / 480.0, 0.5, 4));
	forwardCamera->setPosition_NED(position_NED);
	forwardCamera->setRPY(rpy);

	Matrix3d dcamCal;
	dcamCal << 720,0,0,
			   0,720,0,
			   320,240,1;
	MissionCamera dCam(MissionCameraIDs::Down,
			Vector3d(0.0,-1.0,0.0),	// X vector
			Vector3d(1.0,0.0,0.0),	// Y vector
			Vector3d(0.0,0.0,1.0),	// Z vector
			Vector4d(1.0,0.0,0.0,0.0),	// Quat - populated later by waypoint generator
			Vector2d(325.49416, 222.07906),		// cc
			Vector2d(959.00928, 958.34753),		//fc
			dcamCal);

	downCamera = std::auto_ptr<SimCamera>(new SimCamera(dCam, 37.0, 640.0 / 480.0, 0.5, 4));
	downCamera->setPosition_NED(position_NED);
	downCamera->setRPY(rpy);
}


