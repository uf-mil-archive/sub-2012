#ifndef SIMPIPE_H
#define SIMPIPE_H

#include "SuckySim/SimObject.h"
#include "SuckySim/SimHelpers.h"
#include <Eigen/Dense>
#include <boost/math/constants/constants.hpp>

using namespace Eigen;

namespace subjugator
{
	class SimPipe : public SimObject
	{
	public:
		SimPipe(std::string name, int objectID, QColor color)
			: SimObject(name, objectID, color){}

	protected:
		virtual void Draw(QPainter* painter, double pPerM);
		virtual bool PointIsInside(Vector2d p, double mPerPix);

	private:
		static const double length = 1.2;
		static const double width = 0.15;
	};
}

#endif /* SIMPIPE_H */
