#include "DDSCommanders/PDDDSCommander.h"
#include "SubMain/Workers/PDWorker/SubPDWorker.h"
#include "DataObjects/PD/PDWrench.h"
#include <boost/bind.hpp>

using namespace subjugator;
using namespace boost;

PDDDSCommander::PDDDSCommander(Worker &worker, DDSDomainParticipant *participant)
: wrenchreceiver(participant, "PDWrench", bind(&PDDDSCommander::receivedWrench, this, _1), bind(&PDDDSCommander::writerCountChanged, this, _1)) {
	screwcmdtoken = worker.ConnectToCommand((int)PDWorkerCommands::SetScrew, 5);
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
		shared_ptr<InputToken> ptr = screwcmdtoken.lock();
		if (ptr)
			ptr->Operate(PDWrench(PDWrench::Vector6D::Zero()));
	}
}


