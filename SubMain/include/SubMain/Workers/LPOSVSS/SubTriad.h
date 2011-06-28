#ifndef SUBTRIAD_H
#define SUBTRIAD_H

#include <Eigen/Dense>
#include "SubMain/Workers/LPOSVSS/SubAttitudeHelpers.h"
#include <SubMain/SubPrerequisites.h>

using namespace Eigen;

namespace subjugator
{
	class Triad
	{
	public:
		Triad(const Vector4d& q, const Vector3d& v1_NED, const Vector3d& v2_NED);

		void Update(const Vector3d& v1_BOD, const Vector3d& v2_BOD);

		Vector4d getQuaternion()
		{
			return quaternion;
		}
	private:
		Vector4d quaternion;
		Matrix3d R_NED_T;
	};
}

#endif // SUBTRIAD_H
