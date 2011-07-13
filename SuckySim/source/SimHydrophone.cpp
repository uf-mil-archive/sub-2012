#include "SuckySim/SimHydrophone.h"

using namespace subjugator;
using namespace Eigen;

void SimHydrophone::Draw(QPainter* painter,  double pPerM)
{
	painter->setPen(drawColor);
	painter->setBrush(QBrush(drawColor));

	// Ellipse defined by upper left corner
	int u = (position_NED(0) - diameter / 2.0)*pPerM;
	int v = (position_NED(1) - diameter / 2.0)*pPerM;

	painter->drawEllipse(u, v, diameter*pPerM, diameter*pPerM);
}

bool SimHydrophone::PointIsInside(Vector2d p,  double mPerPix)
{
	Vector2d clickPos = mPerPix*p;

	return SimHelpers::PointInCircle(position_NED.block<2,1>(0,0), diameter, clickPos);
}

HydrophoneInfo SimHydrophone::getHydrophoneInfo(const Vector3d& subPos, const Vector3d& subRPY)
{
	double xyDistance = (position_NED.block<2,1>(0,0) - subPos.block<2,1>(0,0)).norm();
	double zDistance = position_NED(2) - subPos(2);
	double heading = AttitudeHelpers::DAngleDiff(rpy(2), atan2(position_NED(1) - subPos(1), position_NED(0) - subPos(0)));
	double declination = atan2(zDistance, xyDistance);

	HydrophoneInfo info(0,
			(position_NED - subPos).norm(),
			heading,
			declination,
			frequency,
			true
	);

	return info;
}
