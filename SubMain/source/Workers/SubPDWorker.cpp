#include "SubMain/Workers/SubPDWorker.h"

namespace subjugator
{
	PDWorker::PDWorker(boost::asio::io_service& io, int64_t rate)
		: Worker(io, rate)
	{
		mStateManager.SetStateCallback(SubStates::STARTUP,
				STATE_STARTUP_STRING,
				boost::bind(&PDWorker::startupState, this));
		mStateManager.SetStateCallback(SubStates::INITIALIZE,
				STATE_INITIALIZE_STRING,
				boost::bind(&PDWorker::initializeState, this));
		mStateManager.SetStateCallback(SubStates::READY,
				STATE_READY_STRING,
				boost::bind(&PDWorker::readyState, this));
		mStateManager.SetStateCallback(SubStates::STANDBY,
				STATE_STANDBY_STRING,
				boost::bind(&PDWorker::standbyState, this));
		mStateManager.SetStateCallback(SubStates::EMERGENCY,
				STATE_EMERGENCY_STRING,
				boost::bind(&PDWorker::emergencyState, this));
		mStateManager.SetStateCallback(SubStates::FAIL,
				STATE_FAIL_STRING,
				boost::bind(&PDWorker::failState, this));
		mStateManager.SetStateCallback(SubStates::SHUTDOWN,
				STATE_SHUTDOWN_STRING,
				boost::bind(&PDWorker::shutdownState, this));
		mStateManager.SetStateCallback(SubStates::ALL,
				STATE_ALL_STRING,
				boost::bind(&PDWorker::allState, this));

		mStateManager.ChangeState(SubStates::STARTUP);
	}

	void PDWorker::startupState()
	{
		static int count = 0;

		std::cout << "In " << mStateManager.GetStateName(mStateManager.GetCurrentStateCode()) << std::endl;

		if(count++ > 4)
			mStateManager.ChangeState(SubStates::INITIALIZE);
	}

	void PDWorker::initializeState()
	{
		static int count = 0;

		std::cout << "In " << mStateManager.GetStateName(mStateManager.GetCurrentStateCode()) << std::endl;

		if(count++ > 4)
			mStateManager.ChangeState(SubStates::STANDBY);
	}

	void PDWorker::standbyState()
	{
		static int count = 0;

		std::cout << "In " << mStateManager.GetStateName(mStateManager.GetCurrentStateCode()) << std::endl;

		if(count++ > 4)
			mStateManager.ChangeState(SubStates::READY);
	}

	void PDWorker::readyState()
	{
		static int count = 0;

		std::cout << "In " << mStateManager.GetStateName(mStateManager.GetCurrentStateCode()) << std::endl;

		if(count++ > 4)
			mStateManager.ChangeState(SubStates::EMERGENCY);
	}

	void PDWorker::allState()
	{
		static int count = 0;

		std::cout << "In " << "ALL" << std::endl;
	}

	void PDWorker::emergencyState()
	{
		static int count = 0;

		std::cout << "In " << mStateManager.GetStateName(mStateManager.GetCurrentStateCode()) << std::endl;

		if(count++ > 4)
			mStateManager.ChangeState(SubStates::FAIL);
	}

	void PDWorker::failState()
	{
		static int count = 0;

		std::cout << "In " << mStateManager.GetStateName(mStateManager.GetCurrentStateCode()) << std::endl;

		if(count++ > 4)
			mStateManager.ChangeState(SubStates::SHUTDOWN);
	}

	void PDWorker::shutdownState()
	{
		static int count = 0;

		std::cout << "In " << mStateManager.GetStateName(mStateManager.GetCurrentStateCode()) << std::endl;

		if(count++ > 4)
			mStateManager.ChangeState(SubStates::INITIALIZE);
	}
}
