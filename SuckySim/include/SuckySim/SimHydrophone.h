#ifndef SIMHYDROPHONE_H
#define SIMHYDROPHONE_H

#include "SuckySim/SimObject.h"
#include "SuckySim/SimHelpers.h"
#include "DataObjects/Hydrophone/HydrophoneInfo.h"
#include "SubMain/Workers/LPOSVSS/SubAttitudeHelpers.h"

namespace subjugator
{
	class SimHydrophone : public SimObject
	{
	public:
		SimHydrophone(std::string name, int objectID, double freq)
			: SimObject(name, objectID, Qt::black), frequency(freq) {}

		HydrophoneInfo getHydrophoneInfo(const Vector3d& subPos, const Vector3d& subRPY);

	protected:
		virtual void Draw(QPainter* painter, double pPerM);
		virtual bool PointIsInside(Vector2d p, double mPerPix);

	private:
		static const double diameter = 0.2;

		double frequency;
	};
}

#endif /* SIMHYDROPHONE_H */
