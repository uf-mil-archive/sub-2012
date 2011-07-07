#ifndef SUBMISSIONCAMERA_H
#define SUBMISSIONCAMERA_H

#include <Eigen/Dense>

using namespace Eigen;

namespace subjugator
{

	class MissionCameraIDs
	{
	public:
		enum MissionCameraIDCode
		{
			Down = 0,
			Front = 1,
		};
	};

	class MissionCamera
	{
	public:
		MissionCamera(int id, Vector3d x, Vector3d y, Vector3d z, Vector4d quat, Vector2d cc, Vector2d fc)
		: ID(id), X(x), Y(y), Z(z), Quaternion(quat), cc(cc), fc(fc) {}

		int ID;
		Vector3d X;
		Vector3d Y;
		Vector3d Z;
		Vector4d Quaternion;
		Vector2d cc;
		Vector2d fc;
	};
}


#endif /* SUBMISSIONCAMERA_H */
