#ifndef SIMHELPERS_H
#define SIMHELPERS_H

#include <Eigen/Dense>
#include <cmath>

using namespace Eigen;

namespace subjugator
{
	class SimHelpers
	{
	public:
		static bool PointInRectangle(Vector2d rectCenter, double length, double width, double rotRad, Vector2d point);
		static bool PointInCircle(Vector2d circCenter, double diameter, Vector2d point);
	};
}

#endif /* SIMHELPERS_H */
