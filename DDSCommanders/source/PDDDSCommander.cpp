#include "DDSCommanders/PDDDSCommander.h"
#include "SubMain/Workers/PDWorker/SubPDWorker.h"
#include "DataObjects/PD/PDWrench.h"
#include "DataObjects/PD/PDActuator.h"
#include <boost/bind.hpp>
#include <iostream>

using namespace std;
using namespace subjugator;
using namespace boost;

PDDDSCommander::PDDDSCommander(Worker &worker, DDSDomainParticipant *participant)
: wrenchreceiver(participant, "PDWrench", bind(&PDDDSCommander::receivedWrench, this, _1), bind(&PDDDSCommander::writerCountChanged, this, _1)),
  actuatorreceiver(participant, "PDActuator", bind(&PDDDSCommander::receivedActuator, this, _1)) {
	screwcmdtoken = worker.ConnectToCommand((int)PDWorkerCommands::SetScrew, 5);
	actuatorcmdtoken = worker.ConnectToCommand((int)PDWorkerCommands::SetActuator, 5);
}

void PDDDSCommander::receivedWrench(const PDWrenchMessage &wrench) {
	PDWrench::Vector6D vec;
	for (int i=0; i<3; i++)
		vec(i) = wrench.linear[i];
	for (int i=0; i<3; i++)
		vec(i+3) = wrench.moment[i];

	shared_ptr<InputToken> ptr = screwcmdtoken.lock();
	if (ptr)
		ptr->Operate(PDWrench(vec));
}

void PDDDSCommander::writerCountChanged(int count) {
	if (count == 0) {
		cout << "Lost all DataWriters, setting a zero screw" << endl;
		shared_ptr<InputToken> ptr = screwcmdtoken.lock();
		if (ptr)
			ptr->Operate(PDWrench(PDWrench::Vector6D::Zero()));
	}
}

void PDDDSCommander::receivedActuator(const PDActuatorMessage &actuator) {
	shared_ptr<InputToken> ptr = actuatorcmdtoken.lock();
	if (ptr)
		ptr->Operate(PDActuator(actuator.flags));
}

