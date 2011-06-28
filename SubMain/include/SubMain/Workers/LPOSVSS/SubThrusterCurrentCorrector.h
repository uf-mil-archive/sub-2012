#ifndef SUBTHRUSTERCURRENTCORRECTOR_H
#define SUBTHRUSTERCURRENTCORRECTOR_H

#include "SubMain/SubPrerequisites.h"
#include <Eigen/Dense>

namespace subjugator
{
	class ThrusterCurrentCorrector
	{
	public:
		ThrusterCurrentCorrector(int address, const double coeffX[], const double coeffY[],const double coeffZ[]);

		Eigen::Vector3d CalculateDynamicMagCorrection(double current) const;
		static Eigen::Vector3d CalculateTotalCorrection(const std::vector<ThrusterCurrentCorrector>& tList, double currents[]);
	private:
		double coeffX[4];
		double coeffY[4];
		double coeffZ[4];
	};
}


#endif /* SUBTHRUSTERCURRENTCORRECTOR_H */
