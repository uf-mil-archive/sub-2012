#include "SuckySim/SimPipe.h"

using namespace subjugator;
using namespace Eigen;


void SimPipe::Draw(QPainter* painter, double pPerM)
{
	// The center of the rectangle
	int cX = (int)(position_NED(0) * pPerM);
	int cY = (int)(position_NED(1) * pPerM);

	double rotDeg = rpy(2) * 180.0 / boost::math::constants::pi<double>();

	painter->setPen(drawColor);
	painter->setBrush(QBrush(drawColor));

	painter->save();

	painter->translate(QPoint(cX,cY));
	painter->rotate(rotDeg);
	painter->translate(-1.0*(length / 2.0)*pPerM, -1.0*(width / 2.0)*pPerM);	// This translates to the upper left corner
	painter->drawRect(QRect(0,0,length*pPerM, width*pPerM));

	painter->restore();
}

bool SimPipe::PointIsInside(Vector2d p, double mPerPix)
{
	Vector2d clickPos = mPerPix*p;

	return SimHelpers::PointInRectangle(position_NED.block<2,1>(0,0), length, width, rpy(2), clickPos);
}
