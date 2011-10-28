#include "PrimitiveDriver/ThrusterManager.h"
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <string>

using namespace subjugator;
using namespace Eigen;
using namespace boost;
using namespace std;

ThrusterManager::ThrusterManager(HAL &hal, int srcaddress, const ThrusterChangeCallback &callback)
: hal(hal), srcaddress(srcaddress), callback(callback) { }

int ThrusterManager::addThruster(int address) {
	int num = thrusters.size();
	thrusters.push_back(new Thruster(hal, address, srcaddress, bind(callback, num, _1)));
	return num;
}

void ThrusterManager::setEfforts(const VectorXd &efforts) {
	assert(efforts.rows() == thrusters.size());

	for (int i=0; i<efforts.rows(); i++)
		thrusters[i].setEffort(efforts[i]);
}

void ThrusterManager::zeroEfforts() {
	for (ThrusterVec::iterator i = thrusters.begin(); i != thrusters.end(); ++i)
		i->setEffort(0);
}

int ThrusterManager::getOnlineThrusterCount() const {
	int count=0;
	for (ThrusterVec::const_iterator i = thrusters.begin(); i != thrusters.end(); ++i)
		if (i->getWorkerState().code == WorkerState::ACTIVE)
			count++;
	return count;
}

void ThrusterManager::updateState(double dt) {
	state.code = WorkerState::ACTIVE;
	state.msg = lexical_cast<string>(getOnlineThrusterCount()) + " thrusters online";
}

