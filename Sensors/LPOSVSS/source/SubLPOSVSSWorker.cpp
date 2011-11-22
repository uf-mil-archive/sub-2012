#include "LPOSVSS/SubLPOSVSSWorker.h"

using namespace subjugator;
using namespace std;

LPOSVSSWorker::LPOSVSSWorker(boost::asio::io_service& io, int64_t rate, bool useDVL)
	: Worker(io, rate), navComputer(new NavigationComputer(io)), useDVL(useDVL)
{
	mStateManager.SetStateCallback(SubStates::INITIALIZE,
			STATE_INITIALIZE_STRING,
			boost::bind(&LPOSVSSWorker::initializeState, this));
	mStateManager.SetStateCallback(SubStates::READY,
			STATE_READY_STRING,
			boost::bind(&LPOSVSSWorker::readyState, this));
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

	setControlToken((int)LPOSVSSWorkerCommands::SetDepth, boost::bind(&LPOSVSSWorker::setDepth, this, _1));
	setControlToken((int)LPOSVSSWorkerCommands::SetIMU, boost::bind(&LPOSVSSWorker::setIMU, this, _1));
	setControlToken((int)LPOSVSSWorkerCommands::SetDVL, boost::bind(&LPOSVSSWorker::setDVL, this, _1));
	setControlToken((int)LPOSVSSWorkerCommands::SetTare, boost::bind(&LPOSVSSWorker::setTare, this, _1));
	setControlToken((int)LPOSVSSWorkerCommands::SetCurrents, boost::bind(&LPOSVSSWorker::setCurrents, this, _1));
}

/* DO NOT CHANGE CALLBACKS IN HANDLERS! DEADLOCK*/
void LPOSVSSWorker::setDepth(const DataObject& dobj)
{
	const DepthInfo *info = dynamic_cast<const DepthInfo *>(&dobj);
	if(!info)
		return;

	lock.lock();

	depthInfo = std::auto_ptr<DepthInfo>(new DepthInfo(*info));

	lock.unlock();
}

void LPOSVSSWorker::setCurrents(const DataObject& dobj)
{
	return;
}


void LPOSVSSWorker::setIMU(const DataObject& dobj)
{
	const IMUInfo *info = dynamic_cast<const IMUInfo *>(&dobj);
	if(!info)
		return;

	lock.lock();

	imuInfo = std::auto_ptr<IMUInfo>(new IMUInfo(*info));

	lock.unlock();
}

void LPOSVSSWorker::setDVL(const DataObject& dobj)
{
	const DVLHighresBottomTrack *info = dynamic_cast<const DVLHighresBottomTrack *>(&dobj);
	if(!info)
		return;

	lock.lock();

	dvlInfo = std::auto_ptr<DVLHighresBottomTrack>(new DVLHighresBottomTrack(*info));

	lock.unlock();
}

void LPOSVSSWorker::setTare(const DataObject& dobj)
{
	navComputer->TarePosition(Vector3d::Zero());
}

bool LPOSVSSWorker::Startup()
{
	mStateManager.ChangeState(SubStates::INITIALIZE);
	return true;
}

void LPOSVSSWorker::initializeState()
{
	// The packets must be present to start up
	lock.lock();

	if(!imuInfo.get() || !depthInfo.get())
	{
		lock.unlock();
		return;
	}
	cout << "In initialize" << endl;
	if(useDVL)
	{
		cout << "Waiting for good DVL" << endl;
		if(!dvlInfo.get() || !dvlInfo->isGood())
		{
			lock.unlock();
			return;
		}
	}
	else
		cout << "Faking DVL" << endl;

	// Packets are present - go ahead and init
	navComputer->Init(imuInfo, dvlInfo, depthInfo, useDVL);

	lock.unlock();

	// Okay, now flip the callbacks to the navcomputer directly
	changeControlTokenCallback((int)LPOSVSSWorkerCommands::SetDepth, boost::bind(&NavigationComputer::UpdateDepth, boost::ref(*navComputer), _1));
	changeControlTokenCallback((int)LPOSVSSWorkerCommands::SetIMU, boost::bind(&NavigationComputer::UpdateIMU, boost::ref(*navComputer), _1));
	changeControlTokenCallback((int)LPOSVSSWorkerCommands::SetDVL, boost::bind(&NavigationComputer::UpdateDVL, boost::ref(*navComputer), _1));
	changeControlTokenCallback((int)LPOSVSSWorkerCommands::SetCurrents, boost::bind(&NavigationComputer::UpdateCurrents, boost::ref(*navComputer), _1));

	// And push to ready state
	mStateManager.ChangeState(SubStates::READY);
}

void LPOSVSSWorker::readyState()
{
	// publishes in all state
}

void LPOSVSSWorker::allState()
{
	// Get the data
	boost::shared_ptr<LPOSVSSInfo> info(new LPOSVSSInfo(mStateManager.GetCurrentStateCode(), getTimestamp()));

	if(navComputer->getInitialized()) {
		navComputer->GetNavInfo(*info);

		// Emit the LPOSInfo every iteration
		onEmitting(info);
	}
}

void LPOSVSSWorker::emergencyState()
{

}

void LPOSVSSWorker::failState()
{

}

void LPOSVSSWorker::Shutdown()
{
	if(navComputer->getInitialized())
		navComputer->Shutdown();
	shutdown = true;
}

boost::int64_t LPOSVSSWorker::getTimestamp(void)
{
	timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);

	return ((long long int)t.tv_sec * NSEC_PER_SEC) + t.tv_nsec;
}

