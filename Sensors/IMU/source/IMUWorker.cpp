#include "IMU/IMUWorker.h"
#include "IMU/DataObjects/IMUDataObjectFormatter.h"
#include "LibSub/DataObjects/EmbeddedTypeCodes.h"
#include "HAL/format/SPIPacketFormatter.h"

using namespace subjugator;
using namespace boost;
using namespace std;

IMUWorker::IMUWorker(HAL &hal, const WorkerConfigLoader &configloader) : 
Worker("IMU", 5, configloader),
hal(hal),
endpoint(WorkerEndpoint::Args()
	.setName("imu")
	.setEndpoint(hal.makeDataObjectEndpoint(
		getConfig().get<string>("endpoint"),
		new IMUDataObjectFormatter(),
		new SPIPacketFormatter(IMUInfo::PACKET_LENGTH)))
	.setReceiveCallback(bind(&IMUWorker::endpointReceiveCallback, this, _1))
	.setMaxAge(.5))
{
	registerStateUpdater(endpoint);
}

void IMUWorker::endpointReceiveCallback(const boost::shared_ptr<DataObject> &dobj) {
	boost::shared_ptr<IMUInfo> imuinfo = dynamic_pointer_cast<IMUInfo>(dobj);
	if (!imuinfo)
		return;
		
	signal.emit(*imuinfo);
}
