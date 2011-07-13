#ifndef SIMBUOY_H
#define SIMBUOY_H

#include "SuckySim/SimObject.h"
#include "SuckySim/SimHelpers.h"
#include <QtGui>
#include <string>

namespace subjugator
{
	class SimBuoy : public SimObject
	{
	public:
		SimBuoy(std::string name, int objectID, QColor color)
			: SimObject(name, objectID, color){}

	protected:
		virtual void Draw(QPainter* painter, double pPerM);
		virtual bool PointIsInside(Vector2d p, double mPerPix);

	private:
		static const double diameter = 0.23;
	};
}


#endif /* SIMBUOY_H */
