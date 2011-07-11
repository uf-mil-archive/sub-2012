#ifndef SIMWORLD_H
#define SIMWORLD_H

#include <string>
#include <vector>

#include <QtGui>
#include <boost/shared_ptr.hpp>

#include "SuckySim/SimObject.h"

namespace subjugator
{
	class SimWorld
	{
	public:
		SimWorld(std::string name, double length, double width, double depth, int u, int v);

		void setUV(int u, int v) { this->u = u; this->v = v;}

		double getPixelsPerMeter() { return pixelsPerMeter; }
		double getMetersPerPixel() { return metersPerPixel; }

		void AddObjectToWorld(const boost::shared_ptr<SimObject>& obj);

		void Draw(QPainter* painter);

	private:
		std::string name;

		double xdim;
		double ydim;
		double zdim;

		int u;
		int v;

		std::vector<boost::shared_ptr<SimObject> > simObjects;

		double pixelsPerMeter;
		double metersPerPixel;
	};
}

#endif /* SIMWORLD_H */
