#include "PrimitiveDriver/ThrusterMapper.h"
#include <cassert>

using namespace subjugator;
using namespace Eigen;
using namespace std;

ThrusterMapper::ThrusterMapper(const Eigen::Vector3d &centerofmass, int entries)
: centerofmass(centerofmass),
  svdstale(true) { resize(entries); }

void ThrusterMapper::resize(int entries) {
	mapmatrix = MapMatrix::Zero(6, entries);
	fsat = VectorXd::Ones(entries);
	rsat = VectorXd::Ones(entries);
}

void ThrusterMapper::setEntry(int num, const Entry &entry) {
	assert(num < mapmatrix.cols());

	Vector3d moment = (entry.position - centerofmass).cross(entry.lineofaction);

	mapmatrix.col(num) << entry.lineofaction, moment;
	fsat[num] = entry.fsat;
	rsat[num] = entry.rsat;

	svdstale = true;
}

void ThrusterMapper::clearEntry(int num) {
	assert(num < mapmatrix.cols());

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

	// saturate all thrusters independently
	for (int i=0; i<efforts.rows(); i++) {
		efforts[i] = max(min(efforts[i], 1.0), -1.0);
	}

	return efforts;
}

