#include "SubMain/Workers/SubPDWorker.h"

namespace subjugator
{
	PDWorker::PDWorker(boost::asio::io_service& io, int64_t rate)
		: Worker(io, rate)
	{
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
		mStateManager.SetStateCallback(SubStates::ALL,
				STATE_ALL_STRING,
				boost::bind(&PDWorker::allState, this));

		// Set the command vector
		mInputTokenList.resize(3);

		setControlToken((int)PDWorkerCommands::SetWrench, boost::bind(&PDWorker::setWrench, this, _1));
		setControlToken((int)PDWorkerCommands::SetActuator, boost::bind(&PDWorker::setActuator, this, _1));
	}

	void PDWorker::setWrench(const DataObject &obj)
	{
		std::cout << "Setting new wrench!" << std::endl;
	}

	void PDWorker::setActuator(const DataObject &obj)
	{
		std::cout << "Setting actuator!" << std::endl;
	}

	bool PDWorker::Startup()
	{
		mStateManager.ChangeState(SubStates::READY);

		return true;
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

	}
}
