#include "SubMain/Workers/SubDVLWorker.h"

using namespace boost::posix_time;
using namespace boost;

namespace subjugator
{
	DVLWorker::DVLWorker(boost::asio::io_service& io, int64_t rate)
		: Worker(io, rate)
	{
		mStateManager.SetStateCallback(SubStates::READY,
				STATE_READY_STRING,
				boost::bind(&DVLWorker::readyState, this));
		mStateManager.SetStateCallback(SubStates::EMERGENCY,
				STATE_EMERGENCY_STRING,
				boost::bind(&DVLWorker::emergencyState, this));
		mStateManager.SetStateCallback(SubStates::FAIL,
				STATE_FAIL_STRING,
				boost::bind(&DVLWorker::failState, this));
	}

	void DVLWorker::halStateChangeCallback()
	{
		// Do nothing
	}

	void DVLWorker::halReceiveCallback(std::auto_ptr<DataObject> &dobj)
	{
		// Dispatch to the listeners
		onEmitting(boost::shared_ptr<DataObject>(dobj));
	}

	// The worker thread is running when this is called!
	bool DVLWorker::Startup()
	{
		// TODO: pull DVL address from the address config file. it needs an extra column
		// In startup we try to initialize the hal layer. If it fails, we push to fail.
		pEndpoint = hal.openDataObjectEndpoint(50, new DVLDataObjectFormatter(), new DVLPacketFormatter());

		pEndpoint->configureCallbacks(boost::bind(&DVLWorker::halReceiveCallback, this, _1),
									  boost::bind(&DVLWorker::halStateChangeCallback, this));
		pEndpoint->open();

		// Unable to open the endpoint
		if(pEndpoint->getState() == Endpoint::ERROR)
		{
			return false;
		}

		// We're good to go. Start the hal
		hal.startIOThread();

		// The dvl requires a little fiddling to get it rolling
		pEndpoint->write(DVLBreak());	// Send a break
		this_thread::sleep(seconds(2));

		//TODO: Put this into a config file for the worker
		pEndpoint->write(DVLConfiguration(15, 0));	// Send the DVL settings - alignment is handled by LPOS

		// Push to ready
		mStateManager.ChangeState(SubStates::READY);

		return true;
	}

	void DVLWorker::Shutdown()
	{
		// Send a break delimiter to make the dvl stop publishing
		pEndpoint->write(DVLBreak());
	}

	void DVLWorker::readyState()
	{
		// Doesn't do anything, asynchronous callbacks handle it here. This works
		// because the DVL is pushing at the rate we want to publish. No throttling done.
	}

	void DVLWorker::emergencyState()
	{
		// DVL stopped talking for our timeout length. Try to restart it here
	}

	void DVLWorker::failState()
	{
		// Unrecoverable
	}
}

