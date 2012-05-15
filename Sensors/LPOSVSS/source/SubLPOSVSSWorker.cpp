#include "LPOSVSS/SubLPOSVSSWorker.h"
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

using namespace subjugator;
using namespace boost;
using namespace boost::property_tree;
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
	useDVL(useDVL)
{
	registerStateUpdater(dvlmailbox);
	registerStateUpdater(imumailbox);
	registerStateUpdater(depthmailbox);
	registerStateUpdater(currentmailbox);

	const ptree &config = getConfig();
	NavigationComputer::Config navconf;

	navconf.currentconfigs.resize(8);
	const ptree &currentconfigs = config.get_child("currentconfigs");
	for (int i=0; i<8; i++) {
		const ptree &pt = currentconfigs.get_child(lexical_cast<string>(i));

		ThrusterCurrentCorrector::Config &config = navconf.currentconfigs[i];

		const ptree &forward = pt.get_child("forward");
		const ptree &reverse = pt.get_child("reverse");
		for (int j=0; j<4; j++) {
			string jstr = lexical_cast<string>(j);
			config.forward[j] = forward.get<Vector3d>(jstr);
			config.reverse[j] = reverse.get<Vector3d>(jstr);
		}
	}

	navconf.q_MagCorrection = config.get<Vector4d>("q_MagCorrection");
	navconf.magShift = config.get<Vector3d>("magShift");
	navconf.magScale = config.get<Vector3d>("magScale");
	navconf.referenceNorthVector = config.get<Vector3d>("referenceNorthVector");
	navconf.latitudeDeg = config.get<double>("latitudeDeg");
	navconf.dvl_sigma = config.get<Vector3d>("dvl_sigma");
	navconf.att_sigma = config.get<Vector3d>("att_sigma");
	navComputer.reset(new NavigationComputer(navconf));
}

void LPOSVSSWorker::setDepth(const optional<DepthInfo>& info)
{
	if (isActive() && info)
		navComputer->UpdateDepth(*info);
}

void LPOSVSSWorker::setCurrents(const optional<PDInfo>& info)
{
	if (isActive() && info)
		navComputer->UpdateCurrents(*info);
	return;
}

void LPOSVSSWorker::setIMU(const optional<IMUInfo>& info)
{
	if (isActive() && info)
		navComputer->UpdateIMU(*info);
}

void LPOSVSSWorker::setDVL(const optional<DVLVelocity>& info)
{
	if (isActive() && info)
		navComputer->UpdateDVL(*info);
}

void LPOSVSSWorker::enterActive()
{
	navComputer->Init(*imumailbox, *dvlmailbox, *depthmailbox, useDVL);
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
