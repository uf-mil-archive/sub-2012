#ifndef SIMOBJECT_H_
#define SIMOBJECT_H_

#include <Eigen/Dense>
#include <QtGui>
#include <string>

using namespace Eigen;

namespace subjugator
{
	class SimObject
	{
	public:
		SimObject(std::string name, int objectID, QColor color) : name(name),
			objectID(objectID), drawColor(color), position_NED(Vector3d::Zero()),
			quat_NED_B(Vector4d(1.0,0.0,0.0,0.0)), rpy(Vector3d::Zero()){ }

		void setPosition_NED(Vector3d pos) { position_NED = pos; }
		Vector3d getPosition_NED() {return position_NED;}
		void setRPY(Vector3d RPY) { rpy = RPY; }
		Vector3d getRPY() {return rpy;}

		bool getIsHighlighted() { return isHighlighted; }
		void setIsHighlighted(bool value) { isHighlighted = value; }
		int getObjectID() { return objectID; }

		virtual void Draw(QPainter* painter, double pPerM) = 0;
		virtual bool PointIsInside(Vector2d p, double mPerPix) = 0;

	protected:
		std::string name;
		int objectID;
		QColor drawColor;

		Vector3d position_NED;
		Vector4d quat_NED_B;
		Vector3d rpy;

		bool isHighlighted;

	private:
	};
}

#endif /* SIMOBJECT_H_ */
