#ifndef SUBTHRUSTERCURRENTCORRECTOR_H
#define SUBTHRUSTERCURRENTCORRECTOR_H

#include "SubMain/SubPrerequisites.h"
#include <Eigen/Dense>

namespace subjugator
{
	class ThrusterCurrentCorrector
	{
	public:
		ThrusterCurrentCorrector(int address, const double fcoeffX[], const double fcoeffY[], const double fcoeffZ[],
				 const double rcoeffX[], const double rcoeffY[], const double rcoeffZ[]);

		Eigen::Vector3d CalculateDynamicMagCorrection(double current) const;
		static Eigen::Vector3d CalculateTotalCorrection(const std::vector<ThrusterCurrentCorrector>& tList, const std::vector<double>& currents);
	private:
		double fCoeffX[4];
		double fCoeffY[4];
		double fCoeffZ[4];

		double rCoeffX[4];
		double rCoeffY[4];
		double rCoeffZ[4];
	};
}


#endif /* SUBTHRUSTERCURRENTCORRECTOR_H */
