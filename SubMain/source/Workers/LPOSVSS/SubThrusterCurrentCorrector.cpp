#include "SubMain/Workers/LPOSVSS/SubThrusterCurrentCorrector.h"

using namespace subjugator;

ThrusterCurrentCorrector::ThrusterCurrentCorrector(int address, const double coeffX[], const double coeffY[], const double coeffZ[])
{
	for(int i = 0; i < 4; i++)
	{
		this->coeffX[i] = coeffX[i];
		this->coeffY[i] = coeffY[i];
		this->coeffZ[i] = coeffZ[i];
	}
}

Eigen::Vector3d ThrusterCurrentCorrector::CalculateDynamicMagCorrection(double current) const
{
	Eigen::Vector3d res;	// Okay since every element is set below
	double current_squared = current*current;
	double current_cubed = current*current_squared;

	res(0) = coeffX[1] * current + coeffX[2]*current_squared + coeffX[3]*current_cubed;
	res(1) = coeffY[1] * current + coeffY[2]*current_squared + coeffY[3]*current_cubed;
	res(2) = coeffZ[1] * current + coeffZ[2]*current_squared + coeffZ[3]*current_cubed;

	return res;
}

Eigen::Vector3d ThrusterCurrentCorrector::CalculateTotalCorrection(const std::vector<ThrusterCurrentCorrector>& tList,const std::vector<double>& currents)
{
	Eigen::Vector3d res = Eigen::Vector3d::Zero();
	for(size_t i = 0; i < tList.size(); i++)
	{
		res += tList[i].CalculateDynamicMagCorrection(currents[i]);
	}
	return res;
}
