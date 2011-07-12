#include "SuckySim/SimHelpers.h"

using namespace subjugator;
using namespace Eigen;
using namespace std;

bool SimHelpers::PointInRectangle(Vector2d rectCenter, double length, double width, double rotRad, Vector2d point)
{
	rotRad *= -1.0;

	// Rotate the point by the -yaw amount and check against an unrotated rectangle
	Matrix2d rot;
	rot << cos(rotRad), sin(rotRad),
		   -1.0*sin(rotRad), cos(rotRad);

	Vector2d cntr = rot*rectCenter;
	Vector2d pnt = rot*point;

	if((cntr(0) - length / 2.0) < pnt(0) && (cntr(0) + length / 2.0) > pnt(0) &&
	   (cntr(1) - width / 2.0) < pnt(1) && (cntr(1) + width / 2.0) > pnt(1))
		return true;

	return false;
}

bool SimHelpers::PointInCircle(Vector2d circCenter, double diameter, Vector2d point)
{
	double norm = (circCenter - point).norm();

	if(norm <= (diameter / 2.0))
		return true;

	return false;
}
