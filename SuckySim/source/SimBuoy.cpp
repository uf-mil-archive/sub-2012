#include "SuckySim/SimBuoy.h"

using namespace subjugator;
using namespace Eigen;

void SimBuoy::Draw(QPainter* painter,  double pPerM)
{
	painter->setPen(drawColor);
	painter->setBrush(QBrush(drawColor));

	// Ellipse defined by upper left corner
	int u = (position_NED(0) - diameter / 2.0)*pPerM;
	int v = (position_NED(1) - diameter / 2.0)*pPerM;

	painter->drawEllipse(u, v, diameter*pPerM, diameter*pPerM);
}

bool SimBuoy::PointIsInside(Vector2d p,  double mPerPix)
{
	Vector2d clickPos = mPerPix*p;

	return SimHelpers::PointInCircle(position_NED.block<2,1>(0,0), diameter, clickPos);
}
