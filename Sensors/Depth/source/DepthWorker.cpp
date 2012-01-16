#include "Depth/DepthWorker.h"
#include "Depth/DataObjects/DepthDataObjectFormatter.h"
#include "LibSub/DataObjects/EmbeddedTypeCodes.h"
#include "LibSub/DataObjects/HeartBeat.h"
#include "LibSub/DataObjects/StartPublishing.h"
#include "HAL/format/Sub7EPacketFormatter.h"

using namespace subjugator;
using namespace boost;
using namespace std;

DepthWorker::DepthWorker(HAL &hal, const WorkerConfigLoader &configloader) :
Worker("Depth", 5, configloader),
hal(hal),
publishrate(getConfig().get<int>("rate")),
endpoint(WorkerEndpoint::Args()
	.setName("depth")
	.setEndpoint(hal.makeDataObjectEndpoint(
		getConfig().get<string>("endpoint"),
		new DepthDataObjectFormatter(getConfig().get<int>("dest_addr"), getConfig().get<int>("src_addr"), DEPTH),
		new Sub7EPacketFormatter()))
	.setInitCallback(bind(&DepthWorker::endpointInitCallback, this))
	.setReceiveCallback(bind(&DepthWorker::endpointReceiveCallback, this, _1))
	.setMaxAge(1.0/publishrate + .5))
{
	registerStateUpdater(endpoint);
}

void DepthWorker::endpointInitCallback() {
	endpoint.write(HeartBeat());
	endpoint.write(StartPublishing(publishrate));
}

void DepthWorker::endpointReceiveCallback(const boost::shared_ptr<DataObject> &dobj) {
	boost::shared_ptr<DepthInfo> info = dynamic_pointer_cast<DepthInfo>(dobj);
	if (!info)
		return;

	signal.emit(*info);
}
