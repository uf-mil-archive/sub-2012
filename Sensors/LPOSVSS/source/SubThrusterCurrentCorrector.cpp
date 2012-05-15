#include "LPOSVSS/SubThrusterCurrentCorrector.h"

using namespace subjugator;
using namespace Eigen;
using namespace boost;
using namespace std;

ThrusterCurrentCorrector::ThrusterCurrentCorrector(const Config &conf) :
	conf(conf)
{ }

Eigen::Vector3d ThrusterCurrentCorrector::calculate(double current) const
{
	double current_squared = current*current;
	double current_cubed = current*current_squared;

	const array<Vector3d, 4> &coefs = current > 0 ? conf.forward : conf.reverse;

	return coefs[0] + current*coefs[1] + current_squared*coefs[2] + current_cubed*coefs[3];
}

Vector3d ThrusterCurrentCorrector::CalculateTotalCorrection(const vector<ThrusterCurrentCorrector>& tList,const vector<double>& currents) {
	Eigen::Vector3d res = Eigen::Vector3d::Zero();
	for(size_t i = 0; i < tList.size(); i++) {
		res += tList[i].calculate(currents[i]);
	}
	return res;
}
