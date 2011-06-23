#include "SubMain/Workers/SubIMUWorker.h"

namespace subjugator
{
	IMUWorker::IMUWorker(boost::asio::io_service& io, int64_t rate)
		: Worker(io, rate)
	{
		initialized = false;
		mStateManager.SetStateCallback(SubStates::STARTUP,
				STATE_STARTUP_STRING,
				boost::bind(&IMUWorker::startupState, this));
		mStateManager.SetStateCallback(SubStates::READY,
				STATE_READY_STRING,
				boost::bind(&IMUWorker::readyState, this));
		mStateManager.SetStateCallback(SubStates::EMERGENCY,
				STATE_EMERGENCY_STRING,
				boost::bind(&IMUWorker::emergencyState, this));
		mStateManager.SetStateCallback(SubStates::FAIL,
				STATE_FAIL_STRING,
				boost::bind(&IMUWorker::failState, this));

		mStateManager.ChangeState(SubStates::STARTUP);
	}

	void IMUWorker::halStateChangeCallback()
	{
		// Do nothing
	}

	void IMUWorker::halReceiveCallback(std::auto_ptr<DataObject> &dobj)
	{
		// Dispatch to the listeners
		onEmitting(boost::shared_ptr<DataObject>(dobj.get()));
	}

	void IMUWorker::startupState()
	{
		static int tryCount = 0;
		// Only one call allowed past here at a time. I'm not worried about multithreading, this is
		// for sequential state machine invocations
		if(initialized)
			return;

		initialized = true;	// assume success

		// TODO: pull IMU address from the address config file. it needs an extra column
		// In startup we try to initialize the hal layer. If it fails, we push to fail.
		pEndpoint = hal.openDataObjectEndpoint(25, new IMUDataObjectFormatter(), new SPIPacketFormatter(32));

		pEndpoint->configureCallbacks(boost::bind(&IMUWorker::halReceiveCallback,this,_1),
									  boost::bind(&IMUWorker::halStateChangeCallback,this));
		pEndpoint->open();

		// Unable to open the endpoint
		if(pEndpoint->getState() == Endpoint::ERROR)
		{
			// Retry
			initialized = false;
			tryCount++;
			if(tryCount >= 1)	// No retry right now, just fail
				mStateManager.ChangeState(SubStates::FAIL);
		}

		// We're good to go. Push to ready state and start the hal
		hal.startIOThread();
		mStateManager.ChangeState(SubStates::READY);
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

