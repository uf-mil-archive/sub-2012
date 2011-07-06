/*
 * HydrphoneDDSListener.cpp
 *
 *  Created on: Jul 3, 2011
 *      Author: gbdash
 */

#include "DDSListeners/HydrophoneDDSListener.h"
#include "DataObjects/Hydrophone/HydrophoneInfo.h"
#include <iostream>

using namespace subjugator;
using namespace std;

HydrophoneDDSListener::HydrophoneDDSListener(Worker &worker, DDSDomainParticipant *part)
: ddssender(part, "Hydrophone") { connectWorker(worker); }

void HydrophoneDDSListener::DataObjectEmitted(boost::shared_ptr<DataObject> dobj)
{
	// Cast the data object into its real type
	HydrophoneInfo *Hydrophoneinfo = dynamic_cast<HydrophoneInfo *>(dobj.get());
	if (!Hydrophoneinfo)
		return;

	HydrophoneMessage *msg = HydrophoneMessageTypeSupport::create_data();
	msg->timestamp = Hydrophoneinfo->getTimestamp();
	msg->declination = Hydrophoneinfo->getDeclination();
	msg->heading = Hydrophoneinfo->getHeading();
	msg->distance = Hydrophoneinfo->getDistance();
	msg->frequency = Hydrophoneinfo->getPingfrequency();
	msg->valid = Hydrophoneinfo->isValid();

	ddssender.Send(*msg);

	HydrophoneMessageTypeSupport::delete_data(msg);
}

