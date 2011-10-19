/*
 * SubHydrophoneWorker.cpp
 *
 *  Created on: Jul 1, 2011
 *      Author: gbdash
 */

#include "Hydrophone/HydrophoneDataProcessor.h"
#include "Hydrophone/SubHydrophoneWorker.h"
#include "DataObjects/EmbeddedAddresses.h"
#include "DataObjects/Hydrophone/HydrophoneSamples.h"
#include "DataObjects/Hydrophone/HydrophoneInfo.h"
#include "DataObjects/Hydrophone/HydrophoneStart.h"
#include <fstream>

using namespace std;

namespace subjugator
{
	HydrophoneWorker::HydrophoneWorker(boost::asio::io_service& io, int64_t rate, std::string configPath)
		: Worker(io, rate), hal(iothread.getIOService())
	{
		mStateManager.SetStateCallback(SubStates::READY,
				STATE_READY_STRING,
				boost::bind(&HydrophoneWorker::readyState, this));
		mStateManager.SetStateCallback(SubStates::EMERGENCY,
				STATE_EMERGENCY_STRING,
				boost::bind(&HydrophoneWorker::emergencyState, this));
		mStateManager.SetStateCallback(SubStates::FAIL,
				STATE_FAIL_STRING,
				boost::bind(&HydrophoneWorker::failState, this));

		config.load(configPath + string("/hydrophone.config"));

		// Set the command vector
		mInputTokenList.resize(3);

		setControlToken((int)HydrophoneWorkerCommands::logData, boost::bind(&HydrophoneWorker::logNewData, this, _1));
	}

	void HydrophoneWorker::readyState(){}
	void HydrophoneWorker::emergencyState(){}
	void HydrophoneWorker::failState(){}

	//to log samples for debugging uncomment next 2 lines
	ofstream out("logData.csv");
	int packetNo = 0;

	void HydrophoneWorker::halReceiveCallback(std::auto_ptr<DataObject> &dobj)
	{
		HydrophoneSamples *samples = dynamic_cast<HydrophoneSamples *>(dobj.get());
		//cout << samples->getMatrix();
		//to log samples for debugging uncomment lower block
		packetNo++;
		for(int rows = 0; rows < samples->getMatrix().rows(); rows++)
		{
			out << packetNo << ",";
			for(int cols = 0; cols < samples->getMatrix().cols(); cols++)
				out << samples->getMatrix()(rows, cols) << ", ";
			out << endl;
		}

		try {
			HydrophoneDataProcessor proc(samples->getMatrix(), config);
			//cout << fixed << setprecision(16);
			//cout << proc.getDist() << ", " << proc.getHeading()/M_PI*180 << ", " << proc.getDeclination()/M_PI*180 << ", " << proc.getPingfreq() << endl;

			double heading = 0;
			// rotate heading and handle wraparound
			if ((proc.getHeading()/M_PI*180 >= -180.0) && (proc.getHeading()/M_PI*180 <= 90.0))
				heading = -proc.getHeading()/M_PI*180 - 90;
			else if ((proc.getHeading()/M_PI*180 > 90.0) && (proc.getHeading()/M_PI*180 <= 180.0))
				heading = -proc.getHeading()/M_PI*180 + 270;
			else
				heading = 360.0;

			HydrophoneInfo* hydInfo = new HydrophoneInfo(samples->getTimestamp(), proc.getDist(), heading*M_PI/180, proc.getDeclination(), proc.getPingfreq(), proc.isValid());

			// Dispatch to the listeners
			onEmitting(boost::shared_ptr<DataObject>(hydInfo));
		} catch (HydrophoneDataProcessor::Error &err) {
			cout << "Got hydrophone error: " << err.what() << endl;
		}
	}

	void HydrophoneWorker::halStateChangeCallback(){}
	bool HydrophoneWorker::Startup()
	{
		pEndpoint = auto_ptr<DataObjectEndpoint>(hal.openDataObjectEndpoint(HYDROPHONE_ADDR, new HydrophoneDataObjectFormatter(), new HydrophonePacketFormatter()));

		pEndpoint->configureCallbacks(boost::bind(&HydrophoneWorker::halReceiveCallback,this,_1),
			boost::bind(&HydrophoneWorker::halStateChangeCallback,this));

		pEndpoint->open();

		if(pEndpoint->getState() == Endpoint::ERROR)
		{
			return false;
		}

		pEndpoint->write(HydrophoneStart());

		iothread.start();
		mStateManager.ChangeState(SubStates::READY);

		return true;
	}

	void HydrophoneWorker::logNewData(const DataObject& dobj)
	{
		const HydrophoneInfo* hydInfo = dynamic_cast<const HydrophoneInfo *>(&dobj);
		cout << hydInfo->getTimestamp() << ", " << hydInfo->getDistance() << ", " << hydInfo->getHeading()/M_PI*180 << ", " << hydInfo->getDeclination()/M_PI*180 << ", " << hydInfo->getPingfrequency() << ", " << hydInfo->isValid() << endl;
	}
}
