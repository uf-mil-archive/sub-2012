#ifndef SUBMISSIONCAMERA_H
#define SUBMISSIONCAMERA_H

#include <Eigen/Dense>

using namespace Eigen;

namespace subjugator
{
	class MissionCamera
	{
	public:
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
