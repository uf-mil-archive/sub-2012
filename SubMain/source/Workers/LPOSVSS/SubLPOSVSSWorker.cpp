#include "SubMain/Workers/LPOSVSS/SubLPOSVSSWorker.h"

namespace subjugator
{
	LPOSVSSWorker::LPOSVSSWorker(boost::asio::io_service& io, int64_t rate)
		: Worker(io, rate)
	{
		mStateManager.SetStateCallback(SubStates::INITIALIZE,
				STATE_INITIALIZE_STRING,
				boost::bind(&LPOSVSSWorker::initializeState, this));
		mStateManager.SetStateCallback(SubStates::READY,
				STATE_READY_STRING,
				boost::bind(&LPOSVSSWorker::readyState, this));
		mStateManager.SetStateCallback(SubStates::STANDBY,
				STATE_STANDBY_STRING,
				boost::bind(&LPOSVSSWorker::standbyState, this));
		mStateManager.SetStateCallback(SubStates::EMERGENCY,
				STATE_EMERGENCY_STRING,
				boost::bind(&LPOSVSSWorker::emergencyState, this));
		mStateManager.SetStateCallback(SubStates::FAIL,
				STATE_FAIL_STRING,
				boost::bind(&LPOSVSSWorker::failState, this));
		mStateManager.SetStateCallback(SubStates::ALL,
				STATE_ALL_STRING,
				boost::bind(&LPOSVSSWorker::allState, this));

		// Set the command vector
		mInputTokenList.resize(5);

		change = false;
		count5 = false;
		setControlToken((int)LPOSVSSWorkerCommands::SetDepth, boost::bind(&LPOSVSSWorker::setDepth, this, _1));
		//setControlToken((int)PDWorkerCommands::SetScrew, boost::bind(&PDWorker::setScrew, this, _1));
		//setControlToken((int)PDWorkerCommands::SetActuator, boost::bind(&PDWorker::setActuator, this, _1));
	}

	void LPOSVSSWorker::setDepth(const DataObject& obj)
	{
		static int count = 0;


		std::cout << "In first callback!" << std::endl;

		if(count++ > 0)
		{
			change = true;
		}
	}

	void LPOSVSSWorker::setDepth2(const DataObject& obj)
	{
		std::cout << "In second callback!" << std::endl;
	}

	bool LPOSVSSWorker::Startup()
	{
		mStateManager.ChangeState(SubStates::INITIALIZE);
		return true;
	}

	void LPOSVSSWorker::initializeState()
	{
		static int count = 0;
		std::cout << "Working" << std::endl;
		if(change)
		{
			changeControlTokenCallback((int)LPOSVSSWorkerCommands::SetDepth, boost::bind(&LPOSVSSWorker::setDepth2, this, _1));
			change = false;
			count5 = true;
		}
		if(count5)
		{
			if(count++ > 4)
				mStateManager.ChangeState(SubStates::READY);
		}
	}

	void LPOSVSSWorker::standbyState()
	{
		// In standby we don't publish
	}

	void LPOSVSSWorker::readyState()
	{
		// We publish the worker object here
		//onEmitting();

		std::cout << "In Ready " << std::endl;
	}

	void LPOSVSSWorker::allState()
	{

	}

	void LPOSVSSWorker::emergencyState()
	{

	}

	void LPOSVSSWorker::failState()
	{

	}
}
