#include "PrimitiveDriver/ThrusterManager.h"
#include "PrimitiveDriver/DataObjects/MotorDriverDataObjectFormatter.h"
#include "HAL/format/Sub7EPacketFormatter.h"
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <string>

using namespace subjugator;
using namespace Eigen;
using namespace boost;
using namespace std;

ThrusterManager::ThrusterManager(HAL &hal, int srcaddress, const ThrusterChangeCallback &callback)
: hal(hal), srcaddress(srcaddress), callback(callback) { }

int ThrusterManager::addThruster(const std::string &name, const std::string &endpointconf, int address) {
	int num = thrusters.size();

	DataObjectEndpoint *endpoint = hal.makeDataObjectEndpoint(endpointconf, new MotorDriverDataObjectFormatter(address, srcaddress, BRUSHEDOPEN), new Sub7EPacketFormatter());
	thrusters.push_back(new Thruster(name, endpoint, srcaddress));

	return num;
}

void ThrusterManager::setEfforts(const VectorXd &efforts) {
	assert(efforts.rows() == (int)thrusters.size());

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
		if (i->getState().code == State::ACTIVE)
			count++;
	return count;
}

void ThrusterManager::updateState(double dt) {
	for (unsigned int i=0; i<thrusters.size(); i++) {
		Thruster &t = thrusters[i];

		State prevstate = t.getState();
		t.updateState(dt);
		const State &curstate = t.getState();

		if (prevstate != curstate)
			callback(i, curstate);
	}

	state = State(State::ACTIVE, lexical_cast<string>(getOnlineThrusterCount()) + " thrusters online");
}

