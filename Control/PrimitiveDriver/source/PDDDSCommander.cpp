#include "PrimitiveDriver/PDDDSCommander.h"
#include "PrimitiveDriver/PDWorker.h"
#include "DataObjects/PD/PDWrench.h"
#include "DataObjects/Actuator/SetActuator.h"
#include <boost/bind.hpp>
#include <iostream>

using namespace std;
using namespace subjugator;
using namespace boost;

PDDDSCommander::PDDDSCommander(PDWorker &pdworker, DDSDomainParticipant *participant)
: pdworker(pdworker),
  wrenchreceiver(participant, "PDWrench", bind(&PDDDSCommander::receivedWrench, this, _1), bind(&PDDDSCommander::writerCountChanged, this, _1)),
  actuatorreceiver(participant, "PDActuator", bind(&PDDDSCommander::receivedActuator, this, _1)) { }

void PDDDSCommander::receivedWrench(const PDWrenchMessage &wrench) {
	PDWrench::Vector6D vec;
	for (int i=0; i<3; i++)
		vec(i) = wrench.linear[i];
	for (int i=0; i<3; i++)
		vec(i+3) = wrench.moment[i];

	pdworker.setWrench(vec);
}

void PDDDSCommander::writerCountChanged(int count) {
	if (count == 0) {
		cout << "Lost all DataWriters, setting a zero screw" << endl;
		pdworker.setWrench(PDWorker::Vector6d::Zero());
	}
}

void PDDDSCommander::receivedActuator(const PDActuatorMessage &actuator) {
	pdworker.setActuators(actuator.flags);
}

