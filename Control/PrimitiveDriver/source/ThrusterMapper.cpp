#include "PrimitiveDriver/ThrusterMapper.h"
#include <cassert>

using namespace subjugator;
using namespace Eigen;
using namespace std;

ThrusterMapper::ThrusterMapper(const Eigen::Vector3d &centerofmass, int entries)
: centerofmass(centerofmass),
  entries(entries),
  mapmatrix(MapMatrix::Zero(6, entries)),
  fsat(VectorXd::Ones(entries)),
  rsat(VectorXd::Ones(entries)),
  svdstale(true) { }

void ThrusterMapper::resize(int entries) {
	mapmatrix = MapMatrix::Zero(6, entries);
}

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

VectorXd ThrusterMapper::mapWrenchToEfforts(const Vector6D& wrench) const {
	updateSvd();
	VectorXd forces = svd.solve(wrench);
	VectorXd efforts = forcesToEfforts(forces);
	return efforts;
}

void ThrusterMapper::updateSvd() const {
	if (!svdstale)
		return;

	svd.compute(mapmatrix, ComputeThinU | ComputeThinV);
	svdstale = false;
}

VectorXd ThrusterMapper::forcesToEfforts(const VectorXd &forces) const {
	VectorXd efforts(forces.rows());
	for (int i=0; i<forces.rows(); i++) {
		if (forces[i] > 0)
			efforts[i] = forces[i] / fsat[i];
		else
			efforts[i] = forces[i] / rsat[i];
	}

	double maxeffort = efforts.array().abs().maxCoeff();
	if (maxeffort > 1.0)
		efforts /= maxeffort;

	return efforts;
}

