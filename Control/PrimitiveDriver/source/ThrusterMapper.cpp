#include "PrimitiveDriver/ThrusterMapper.h"
#include <cassert>

using namespace subjugator;
using namespace Eigen;
using namespace std;

ThrusterMapper::ThrusterMapper(const Eigen::Vector3d &centerofmass, int entries)
: centerofmass(centerofmass),
  entries(entries),
  mapmatrix(MapMatrix::Zero(entries, 6)),
  fsat(VectorXd::Ones(entries)),
  rsat(VectorXd::Ones(entries)),
  svdstale(true) { }

void ThrusterMapper::setEntry(int num, const Entry &entry) {
	assert(num < entries);

	Vector3d moment = (entry.position - centerofmass).cross(entry.lineofaction);

	mapmatrix.block<3, 1>(0, num) = entry.lineofaction;
	mapmatrix.block<3, 1>(3, num) = moment;
	fsat[num] = entry.fsat;
	rsat[num] = entry.rsat;

	svdstale = true;
}

void ThrusterMapper::clearEntry(int num) {
	assert(num < entries);

	mapmatrix.col(num).fill(0);
	svdstale = true;
}

VectorXd ThrusterMapper::mapWrench(const Vector6D& wrench) const {
	updateSvd();
	VectorXd forces = svd.solve(wrench);
	saturate(forces);
	return forces;
}

void ThrusterMapper::updateSvd() const {
	if (!svdstale)
		return;

	svd.compute(mapmatrix, ComputeThinU | ComputeThinV);
	svdstale = false;
}

void ThrusterMapper::saturate(VectorXd &forces) const {
	double maxnorm = forces.cwiseQuotient(fsat).maxCoeff(); // find the maximum force normalized by forward saturation
	double minnorm = forces.cwiseQuotient(rsat).minCoeff(); // find the minimum force normalized by reverse saturation

	if (maxnorm > 1.0 || minnorm < -1.0) // if the highest force exceeds saturation in either forward or reverse
		forces /= max(maxnorm, -minnorm); // scale everything by the greatest normalized force
}

