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
	killmon("HOB", bind(&DVLWorker::killChangedCallback, this)),
	hobkillsignal("HOB", "DVL's minimum height over bottom kill. To unkill, restart DVL or toggle another kill source"),
	endpoint(WorkerEndpoint::Args()
	         .setName("dvl")
	         .setEndpoint(hal.makeDataObjectEndpoint(getConfig().get<string>("endpoint"),
	                                                 new DVLDataObjectFormatter(),
	                                                 new DVLPacketFormatter()))
	         .setInitCallback(bind(&DVLWorker::endpointInitCallback, this))
	         .setReceiveCallback(bind(&DVLWorker::endpointReceiveCallback, this, _1))),
	sentconf(false),
	badpingerrorcount(getConfig().get<int>("bad_ping_error_count")),
	badpingctr(0),
	badhoberrorcount(getConfig().get<int>("bad_hob_error_count")),
	badhobctr(0),
	stateupdater(*this)
{
	registerStateUpdater(endpoint);
	registerStateUpdater(stateupdater);
}

void DVLWorker::enterActive() {
	hobkillsignal.unkill();
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

		updateHOBKill(info->height);
	} else if (boost::shared_ptr<DVLStartupBanner> startup = dynamic_pointer_cast<DVLStartupBanner>(dobj)) {
		endpoint.write(DVLConfiguration(getConfig().get<double>("max_depth")));
		endpoint.setMaxAge(.5);

		if (sentconf)
			logger.log("Unexpected reset", WorkerLogEntry::ERROR);
		sentconf = true;
	}
}

void DVLWorker::updateHOBKill(const boost::optional<double> &height) {
	if (height) {
		if (height < getConfig().get<double>("min_hob")) {
			if (++badhobctr == badhoberrorcount) {
				logger.log("Kill activated because HOB fell below the minimum safe distance. To unkill sub, restart DVL worker or toggle another kill source");
				hobkillsignal.kill();
			}
		} else {
			badhobctr = 0;
		}
	} else if (badhobctr > 1) {
		logger.log("Kill activated because DVL dropped out after HOB momentarily fell below minimum safe distance. To unkill sub, restart DVL worker or toggle another kill source");
		hobkillsignal.kill();
	}
}

void DVLWorker::killChangedCallback() {
	if (killmon.isKilled() && hobkillsignal.isKilled()) {
		logger.log("HOB kill deactivated");
		hobkillsignal.unkill();
		badhobctr = 0;
	}
}

void DVLWorker::ConfStateUpdater::updateState(double dt) {
	if (worker.sentconf)
		state = State::ACTIVE;
	else
		state = State(State::STANDBY, "Not configured");
}
