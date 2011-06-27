#ifndef SUBTRIAD_H
#define SUBTRIAD_H

#include <Eigen/Dense>
#include "SubMain/Workers/LPOSVSS/SubAttitudeHelpers.h"

using namespace Eigen;

namespace subjugator
{
	class Triad
	{
	public:
		Triad(const Vector4d& q, const Vector3d& v1_NED, const Vector3d& v2_NED);

		Vector4d Update(const Vector3d& v1_BOD, const Vector3d& v2_BOD);

	private:
		Vector4d Quaternion;
		Matrix3d R_NED_T;
	};
}

#endif // SUBTRIAD_H
