#include "IMUSmartSensor/SubIMUWorker.h"
#include <iostream>

using namespace std;

namespace subjugator
{
	IMUWorker::IMUWorker(boost::asio::io_service& io, int64_t rate)
		: Worker(io, rate), hal(iothread.getIOService())
	{
		mStateManager.SetStateCallback(SubStates::READY,
				STATE_READY_STRING,
				boost::bind(&IMUWorker::readyState, this));
		mStateManager.SetStateCallback(SubStates::EMERGENCY,
				STATE_EMERGENCY_STRING,
				boost::bind(&IMUWorker::emergencyState, this));
		mStateManager.SetStateCallback(SubStates::FAIL,
				STATE_FAIL_STRING,
				boost::bind(&IMUWorker::failState, this));
	}

	void IMUWorker::halStateChangeCallback()
	{
		// Do nothing
	}

	void IMUWorker::halReceiveCallback(std::auto_ptr<DataObject> &dobj)
	{
		// Dispatch to the listeners
		onEmitting(boost::shared_ptr<DataObject>(dobj));
	}

	bool IMUWorker::Startup()
	{
		// TODO: pull IMU address from the address config file. it needs an extra column
		// In startup we try to initialize the hal layer. If it fails, we push to fail.
		pEndpoint = std::auto_ptr<DataObjectEndpoint>(hal.openDataObjectEndpoint(25, new IMUDataObjectFormatter(), new SPIPacketFormatter(32)));

		pEndpoint->configureCallbacks(boost::bind(&IMUWorker::halReceiveCallback,this,_1),
									  boost::bind(&IMUWorker::halStateChangeCallback,this));
		pEndpoint->open();

		// Unable to open the endpoint
		if(pEndpoint->getState() == Endpoint::ERROR)
		{
			// Retry?
			return false;
		}

		// We're good to go. Push to ready state and start the hal
		iothread.start();
		mStateManager.ChangeState(SubStates::READY);

		return true;
	}

	void IMUWorker::readyState()
	{
		// Doesn't do anything, asynchronous callbacks handle it here. This works
		// because the IMU is pushing at the rate we want to publish. No throttling done.
	}

	void IMUWorker::emergencyState()
	{

	}

	void IMUWorker::failState()
	{

	}
}

