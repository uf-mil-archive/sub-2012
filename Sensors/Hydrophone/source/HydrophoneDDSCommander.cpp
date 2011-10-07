/*
 * HydrophoneDDSCommander.cpp
 *
 *  Created on: Jul 4, 2011
 *      Author: gbdash
 */

#include "Hydrophone/HydrophoneDDSCommander.h"
#include "Hydrophone/SubHydrophoneWorker.h"
#include "DataObjects/Hydrophone/HydrophoneInfo.h"
#include <boost/bind.hpp>

using namespace subjugator;
using namespace boost;

HydrophoneDDSCommander::HydrophoneDDSCommander(Worker &worker, DDSDomainParticipant *participant)
: hydrophoneReceiver(participant, "Hydrophone", bind(&HydrophoneDDSCommander::receivedHydrophoneInfo, this, _1)) {
	hydcmdtoken = worker.ConnectToCommand(HydrophoneWorkerCommands::logData, 5);
}

void HydrophoneDDSCommander::receivedHydrophoneInfo(const HydrophoneMessage &msg) {
	hydcmdtoken.lock()->Operate(HydrophoneInfo(msg.timestamp, msg.distance, msg.heading, msg.declination, msg.frequency, msg.valid));
}
