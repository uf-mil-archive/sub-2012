#include "Hydrophone/HydrophoneDataProcessor.h"
#include "Hydrophone/HydrophoneWorker.h"
#include "Hydrophone/DataObjects/HydrophonePacketFormatter.h"
#include "Hydrophone/DataObjects/HydrophoneDataObjectFormatter.h"
#include "Hydrophone/DataObjects/HydrophoneSamples.h"
#include "Hydrophone/DataObjects/HydrophoneStart.h"
#include "LibSub/Math/EigenUtils.h"
#include <fstream>

using namespace subjugator;
using namespace boost;
using namespace boost::property_tree;
using namespace Eigen;
using namespace std;

HydrophoneWorker::HydrophoneWorker(HAL &hal, const WorkerConfigLoader &configloader) :
Worker("Hydrophone", 1, configloader),
hal(hal),
endpoint(WorkerEndpoint::Args()
	.setName("hydrophone")
	.setEndpoint(hal.makeDataObjectEndpoint(
		getConfig().get<std::string>("endpoint"),
		new HydrophoneDataObjectFormatter(),
		new HydrophonePacketFormatter()))
	.setInitCallback(bind(&HydrophoneWorker::endpointInitCallback, this))
	.setReceiveCallback(bind(&HydrophoneWorker::endpointReceiveCallback, this, _1))
) {
	const ptree &config = getConfig();
	dpconfig.scalefact = config.get<int>("scalefact");
	dpconfig.samplingrate = config.get<int>("samplingrate");
	dpconfig.zerocount = config.get<int>("zerocount");
	dpconfig.fftsize = config.get<int>("fftsize");
	dpconfig.soundvelocity = config.get<double>("soundvelocity");
	dpconfig.disth = config.get<double>("disth");
	dpconfig.disth4 = config.get<double>("disth4");
	dpconfig.bandpass_fircoefs = config.get<VectorXd>("bandpass");
	dpconfig.upsample_fircoefs = config.get<VectorXd>("upsample");
	dpconfig.hamming_fircoefs = config.get<VectorXd>("hamming");

	registerStateUpdater(endpoint);
}

void HydrophoneWorker::endpointInitCallback() {
	endpoint.write(HydrophoneStart());
}

void HydrophoneWorker::endpointReceiveCallback(const boost::shared_ptr<DataObject> &dobj) {
	boost::shared_ptr<HydrophoneSamples> samples = dynamic_pointer_cast<HydrophoneSamples>(dobj);
	if (!samples)
		return;

	ofstream out("samples.dat");
	for (int i=0; i<samples->getMatrix().rows(); i++) {
		for (int j=0; j<4; j++) {
			if (j != 0)
				out << ", ";
			out << samples->getMatrix()(i, j);
		}
		out << "\n";
	}

	try {
		HydrophoneDataProcessor proc(samples->getMatrix(), dpconfig);

		double heading = -proc.getHeading() - M_PI/2;
		while (heading > M_PI)
			heading -= M_PI*2;
		while (heading < -M_PI)
			heading += M_PI*2;

		Info info = { samples->getTimestamp(), proc.getDist(), heading, proc.getDeclination(), proc.getPingfreq(), proc.isValid() };
		signal.emit(info);
	} catch (HydrophoneDataProcessor::Error &err) {
		logger.log(string("Error processing ping: ") + err.what(), WorkerLogEntry::ERROR);
	}
}
