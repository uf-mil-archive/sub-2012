#include "LPOSVSS/SubThrusterCurrentCorrector.h"

using namespace subjugator;

ThrusterCurrentCorrector::ThrusterCurrentCorrector(int address, const double fcoeffX[], const double fcoeffY[], const double fcoeffZ[],
		 const double rcoeffX[], const double rcoeffY[], const double rcoeffZ[])
{
	for(int i = 0; i < 4; i++)
	{
		this->fCoeffX[i] = fcoeffX[i];
		this->fCoeffY[i] = fcoeffY[i];
		this->fCoeffZ[i] = fcoeffZ[i];

		this->rCoeffX[i] = rcoeffX[i];
		this->rCoeffY[i] = rcoeffY[i];
		this->rCoeffZ[i] = rcoeffZ[i];
	}
}

Eigen::Vector3d ThrusterCurrentCorrector::CalculateDynamicMagCorrection(double current) const
{
	Eigen::Vector3d res;	// Okay since every element is set below
	double current_squared = current*current;
	double current_cubed = current*current_squared;

	if(current >= 0)
	{
		res(0) = fCoeffX[1] * current + fCoeffX[2]*current_squared + fCoeffX[3]*current_cubed;
		res(1) = fCoeffY[1] * current + fCoeffY[2]*current_squared + fCoeffY[3]*current_cubed;
		res(2) = fCoeffZ[1] * current + fCoeffZ[2]*current_squared + fCoeffZ[3]*current_cubed;
	}
	else
	{
		res(0) = rCoeffX[1] * current + rCoeffX[2]*current_squared + rCoeffX[3]*current_cubed;
		res(1) = rCoeffY[1] * current + rCoeffY[2]*current_squared + rCoeffY[3]*current_cubed;
		res(2) = rCoeffZ[1] * current + rCoeffZ[2]*current_squared + rCoeffZ[3]*current_cubed;
	}

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
