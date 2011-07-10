#include "SuckySim/SimSub.h"

using namespace subjugator;
using namespace Eigen;
using namespace std;

SimSub::SimSub(std::string name, int objectID, QColor color)
	: SimObject(name, objectID, color), length(1.27),
	  width(0.4572), podLength(0.8175), podWidth(0.1143),
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

void SimSub::Draw(QPainter* painter,  double pPerM)
{
	// Positions for sub

	// The center point of the sub rectangle
	int cX = (int)(position_NED(0) * pPerM);
	int cY = (int)(position_NED(1) * pPerM);

	// Positions for Left pod
	// Calculate the left pod back point connected to the hull
	QPointF *lPod1 = new QPointF
	(
		0 + (((length-podLength)/2)*pPerM),
		0
	);

	// Calculate the left pod back point not connected to hull
	QPointF *lPod2 = new QPointF
	(
		lPod1->x(),
		lPod1->y() - ((podWidth) * pPerM)
	);

	// Calculate the left pod front point not connected to hull
	QPointF *lPod3 = new QPointF
	(
		((podLength) * pPerM),
		lPod2->y()
	);

	// Calculate the left pod front point connected to hull
	QPointF *lPod4 = new QPointF
	(
		lPod3->x(),
		lPod1->y()
	);



	// Positions for right pod
	// Calculate the right pod back point connected to the hull
	QPointF *rPod1 = new QPointF
	(
		0 + (((length-podLength)/2)*pPerM),
		(width * pPerM)
	);

	// Calculate the right pod back point not connected to hull
	QPointF *rPod2 = new QPointF
	(
		rPod1->x(),
		rPod1->y() + ((podWidth) * pPerM)
	);

	// Calculate the right pod front point not connected to hull
	QPointF *rPod3 = new QPointF
	(
		(podLength * pPerM),
		rPod2->y()
	);

	// Calculate the right pod front point connected to hull
	QPointF *rPod4 = new QPointF
	(
		rPod3->x(),
		rPod1->y()
	);

	double rotDeg = rpy(2) * 180.0 / boost::math::constants::pi<double>();

	painter->setPen(drawColor);
	painter->setBrush(QBrush(drawColor));

	painter->save();

	painter->translate(QPoint(cX,cY));
	painter->rotate(rotDeg);
	painter->translate(-1.0*(length / 2.0)*pPerM, -1.0*(width / 2.0)*pPerM);	// This translates to the upper left corner

	// Draw Body
	painter->drawRect(QRect(0,0,length*pPerM, width*pPerM));

	// Draw the left pod
	painter->drawRect(QRect(((length-podLength)/2)*pPerM,-podWidth*pPerM,podLength*pPerM, podWidth*pPerM));
//	painter->drawLine(QLineF(*lPod1, *lPod2));
//	painter->drawLine(QLineF(*lPod2, *lPod3));
//	painter->drawLine(QLineF(*lPod3, *lPod4));

	// Draw the right pod
	painter->drawRect(QRect(((length-podLength)/2)*pPerM,width*pPerM,podLength*pPerM, podWidth*pPerM));
//	painter->drawLine(QLineF(*rPod1, *rPod2));
//	painter->drawLine(QLineF(*rPod2, *rPod3));
//	painter->drawLine(QLineF(*rPod3, *rPod4));

	painter->restore();
}

bool SimSub::PointIsInside(Vector2d p, double mPerPix)
{
	Vector2d clickPos = mPerPix*p;

	return SimHelpers::PointInRectangle(position_NED.block<2,1>(0,0), length, width, rpy(2), clickPos);
}
