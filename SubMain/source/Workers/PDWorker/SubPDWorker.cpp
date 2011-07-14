#include "SubMain/Workers/PDWorker/SubPDWorker.h"
#include "DataObjects/PD/PDWrench.h"
#include "DataObjects/PD/PDInfo.h"
#include "DataObjects/Actuator/SetActuator.h"
#include <boost/bind.hpp>

using namespace boost;

namespace subjugator
{//TODO address hardcoding = not cool
	PDWorker::PDWorker(boost::asio::io_service& io, int64_t rate)
		: Worker(io, rate),hal(new SubHAL()),
		hbeatEndpoint(hal->openDataObjectEndpoint(255, new MotorDriverDataObjectFormatter(255, 21, HEARTBEAT), new Sub7EPacketFormatter())),
		mergeManager(*hal)
	{
		hbeatEndpoint->open();
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

		setControlToken((int)PDWorkerCommands::SetScrew, boost::bind(&PDWorker::setScrew, this, _1));
		setControlToken((int)PDWorkerCommands::SetActuator, boost::bind(&PDWorker::setActuator, this, _1));
	}

	void PDWorker::setScrew(const DataObject &obj)
	{
		if (const PDWrench *wrench = dynamic_cast<const PDWrench *>(&obj))
		{
			thrusterManager->ImplementScrew(wrench->getVec());
		}
	}

	void PDWorker::setActuator(const DataObject &obj)
	{
		if (const SetActuator *actuator = dynamic_cast<const SetActuator *>(&obj))
		{
			mergeManager.setActuators(actuator->getFlags());
		}
	}

	bool PDWorker::Startup()
	{
		// Build the ThrusterManager
		thrusterManager = std::auto_ptr<ThrusterManager>(new ThrusterManager(hal));

		mStateManager.ChangeState(SubStates::INITIALIZE);
		hal->startIOThread();

		return true;
	}

	void PDWorker::initializeState()
	{
		if(thrusterManager->IsReady())
			mStateManager.ChangeState(SubStates::READY);
	}

	void PDWorker::standbyState()
	{
		// In standby we don't publish
	}

	void PDWorker::readyState()
	{
		std::vector<double> currents(8);
		for (int i=0; i<8; i++) {
			currents[i] = thrusterManager->getCurrent(i);
		}

		onEmitting(boost::shared_ptr<DataObject>(new PDInfo(mStateManager.GetCurrentStateCode(), getTimestamp(), currents, mergeManager.getMergeInfo())));
	}

	void PDWorker::allState()
	{
		// push out the heartbeat
		hbeatEndpoint->write(HeartBeat());
	}

	void PDWorker::emergencyState()
	{

	}

	void PDWorker::failState()
	{

	}
}
