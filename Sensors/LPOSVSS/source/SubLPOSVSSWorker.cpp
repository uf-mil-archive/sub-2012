#include "LPOSVSS/SubLPOSVSSWorker.h"
#include <boost/bind.hpp>

using namespace subjugator;
using namespace boost;
using namespace std;

LPOSVSSWorker::LPOSVSSWorker(const WorkerConfigLoader &configloader) :
	Worker("LPOSVSS", 200, configloader),
	dvlmailbox(WorkerMailbox<DVLVelocity>::Args()
	           .setName("dvl")
	           .setCallback(bind(&LPOSVSSWorker::setDVL, this, _1))),
	imumailbox(WorkerMailbox<IMUInfo>::Args()
	           .setName("imu")
	           .setCallback(bind(&LPOSVSSWorker::setIMU, this, _1))),
	depthmailbox(WorkerMailbox<DepthInfo>::Args()
	             .setName("depth")
	             .setCallback(bind(&LPOSVSSWorker::setDepth, this, _1))),
	currentmailbox(WorkerMailbox<PDInfo>::Args()
	               .setName("current")
	               .setCallback(bind(&LPOSVSSWorker::setCurrents, this, _1))),
	navComputer(new NavigationComputer()),
	useDVL(useDVL)
{
	registerStateUpdater(dvlmailbox);
	registerStateUpdater(imumailbox);
	registerStateUpdater(depthmailbox);
	registerStateUpdater(currentmailbox);
}

void LPOSVSSWorker::setDepth(const optional<DepthInfo>& info)
{
	if (info) {
		depthInfo = std::auto_ptr<DepthInfo>(new DepthInfo(*info));
		navComputer->UpdateDepth(*info);
	}
}

void LPOSVSSWorker::setCurrents(const optional<PDInfo>& info)
{
	if (isActive())
		navComputer->UpdateCurrents(*info);
	return;
}

void LPOSVSSWorker::setIMU(const optional<IMUInfo>& info)
{
	if (info) {
		imuInfo = std::auto_ptr<IMUInfo>(new IMUInfo(*info));
		if (isActive())
			navComputer->UpdateIMU(*info);
	}
}

void LPOSVSSWorker::setDVL(const optional<DVLVelocity>& info)
{
	if (info) {
		dvlInfo = std::auto_ptr<DVLVelocity>(new DVLVelocity(*info));
		if (isActive())
			navComputer->UpdateDVL(*info);
	}

}

void LPOSVSSWorker::enterActive()
{
	navComputer->Init(imuInfo, dvlInfo, depthInfo, useDVL);
}

void LPOSVSSWorker::work(double dt)
{
	LPOSVSSInfo info;

	navComputer->Update((boost::int64_t)(dt * 1000));
	navComputer->GetNavInfo(info);

	// Emit the LPOSInfo every iteration
	signal.emit(info);
}

LPOSVSSWorker::~LPOSVSSWorker()
{
	if(navComputer->getInitialized())
		navComputer->Shutdown();
}

boost::int64_t LPOSVSSWorker::getTimestamp(void)
{
	timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);

	static const uint64_t NSEC_PER_SEC = 1000000000;
	return ((long long int)t.tv_sec * NSEC_PER_SEC) + t.tv_nsec;
}

