#include "SubMain/Workers/SubDepthWorker.h"

namespace subjugator
{
	DepthWorker::DepthWorker(boost::asio::io_service& io, int64_t rate)
		: Worker(io, rate)
	{
		initialized = false;
		mStateManager.SetStateCallback(SubStates::STARTUP,
				STATE_STARTUP_STRING,
				boost::bind(&DepthWorker::startupState, this));
		mStateManager.SetStateCallback(SubStates::READY,
				STATE_READY_STRING,
				boost::bind(&DepthWorker::readyState, this));
		mStateManager.SetStateCallback(SubStates::EMERGENCY,
				STATE_EMERGENCY_STRING,
				boost::bind(&DepthWorker::emergencyState, this));
		mStateManager.SetStateCallback(SubStates::FAIL,
				STATE_FAIL_STRING,
				boost::bind(&DepthWorker::failState, this));
		mStateManager.SetStateCallback(SubStates::SHUTDOWN,
				STATE_SHUTDOWN_STRING,
				boost::bind(&DepthWorker::shutdownState, this));

		mStateManager.ChangeState(SubStates::STARTUP);
	}

	void DepthWorker::halStateChangeCallback()
	{
		// Do nothing
	}

	void DepthWorker::halReceiveCallback(std::auto_ptr<DataObject> &dobj)
	{
		// Dispatch to the listeners
		onEmitting(boost::shared_ptr<DataObject>(dobj.get()));
	}

	// The worker thread is running when this is called!
	void DepthWorker::startupState()
	{
		static int tryCount = 0;
		// Only one call allowed past here at a time. I'm not worried about multithreading, this is
		// for sequential state machine invocations
		if(initialized)
			return;

		initialized = true;	// assume success

		// TODO: pull Depth address from the address config file. it needs an extra column
		// In startup we try to initialize the hal layer. If it fails, we push to fail.
		pEndpoint = hal.openDataObjectEndpoint(4, new DepthDataObjectFormatter(DEPTH_ADDR, GUMSTIX_ADDR, DEPTH), new Sub7EPacketFormatter());

		pEndpoint->configureCallbacks(boost::bind(&DepthWorker::halReceiveCallback,this,_1),
									  boost::bind(&DepthWorker::halStateChangeCallback,this));
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

		// We're good to go. Start the hal
		hal.startIOThread();

		// To get the depth running, we require an initial heartbeat BEFORE telling it to publish
		pEndpoint->write(HeartBeat());

		// TODO Config file for publish rate?
		pEndpoint->write(StartPublishing(20));

		// Push to ready
		mStateManager.ChangeState(SubStates::READY);
	}

	void DepthWorker::readyState()
	{
		// The depth publishes a heartbeat in ready state
		pEndpoint->write(HeartBeat());
	}

	void DepthWorker::emergencyState()
	{

	}

	void DepthWorker::failState()
	{

	}

	void DepthWorker::shutdownState()
	{
		// Tell the depth board to stop publishing
		pEndpoint->write(StopPublishing());
	}
}

