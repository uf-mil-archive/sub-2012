#include "SuckySim/SimWorld.h"

using namespace subjugator;

SimWorld::SimWorld(std::string name, double length, double width, double depth, int u, int v)
: name(name), xdim(length), ydim(width), zdim(depth), u(u), v(v)
{
	if(u <= v)
	{
		pixelsPerMeter = (double)u / xdim;
		metersPerPixel = xdim / (double)u;
	}
	else
	{
		pixelsPerMeter = (double)v / ydim;
		metersPerPixel = ydim / (double)v;
	}
}

void SimWorld::Draw(QPainter* painter)
{
	for(size_t i = 0; i < simObjects.size(); i++)
		simObjects[i]->Draw(painter, pixelsPerMeter);
}

void SimWorld::AddObjectToWorld(const boost::shared_ptr<SimObject>& obj)
{
	simObjects.push_back(obj);
}
