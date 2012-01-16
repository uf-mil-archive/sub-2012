#include "DVL/DVLWorker.h"
#include "DVL/DataObjects/DVLDataObjectFormatter.h"
#include "DVL/DataObjects/DVLPacketFormatter.h"
#include "DVL/DataObjects/DVLBreak.h"
#include "DVL/DataObjects/DVLConfiguration.h"
#include <boost/thread.hpp>

using namespace subjugator;
using namespace boost::posix_time;
using namespace boost::property_tree;
using namespace boost;
using namespace std;

DVLWorker::DVLWorker(HAL &hal, const WorkerConfigLoader &configloader) :
Worker("DVL", 10, configloader),
endpoint(WorkerEndpoint::Args()
	.setName("dvl")
	.setEndpoint(hal.makeDataObjectEndpoint(
		getConfig().get<string>("endpoint"),
		new DVLDataObjectFormatter(),
		new DVLPacketFormatter()))
	.setInitCallback(bind(&DVLWorker::endpointInitCallback, this))
	.setReceiveCallback(bind(&DVLWorker::endpointReceiveCallback, this, _1))),
badpingerrorcount(getConfig().get<int>("bad_ping_error_count")),
sentconf(false),
badpingctr(0),
stateupdater(*this)
{
	registerStateUpdater(endpoint);
	registerStateUpdater(stateupdater);
}

DVLWorker::~DVLWorker() {
	endpoint.write(DVLBreak());
}

void DVLWorker::endpointInitCallback() {
	endpoint.write(DVLBreak());
	endpoint.clearMaxAge();
}

void DVLWorker::endpointReceiveCallback(const boost::shared_ptr<DataObject> &dobj) {
	if (boost::shared_ptr<DVLInfo> info = dynamic_pointer_cast<DVLInfo>(dobj)) {
		if (!sentconf)
			return;
			
		signal.emit(*info);
		
		if (info->velocity) {
			if (badpingctr >= badpingerrorcount)
				logger.log("Bottom tracking returned");
			badpingctr = 0;
		} else {
			if (++badpingctr == badpingerrorcount)
				logger.log("Bottom tracking dropped out", WorkerLogEntry::ERROR);
		}
	} else if (boost::shared_ptr<DVLStartupBanner> startup = dynamic_pointer_cast<DVLStartupBanner>(dobj)) {
		const ptree &config = getConfig();
		endpoint.write(DVLConfiguration(config.get<double>("max_depth")));
		endpoint.setMaxAge(.5);
		
		if (sentconf)
			logger.log("Unexpected reset", WorkerLogEntry::ERROR);
		sentconf = true;
	}
}

void DVLWorker::ConfStateUpdater::updateState(double dt) {
	if (worker.sentconf)
		state = State::ACTIVE;
	else
		state = State(State::STANDBY, "Not configured");
}
