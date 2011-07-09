#include <iostream>
#include <vector>
#include <complex>
#include <fstream>
#include <Eigen/Dense>
#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <unsupported/Eigen/FFT>
#include <iomanip>
#include "Hydrophone/HydrophoneDataProcessor.h"
#include "Hydrophone/SubHydrophoneWorker.h"
#include "config.h"
#include "DDSListeners/HydrophoneDDSListener.h"
#include "DDSCommanders/HydrophoneDDSCommander.h"
#include <ndds/ndds_cpp.h>

using namespace std;
using namespace subjugator;
using namespace boost;
using namespace Eigen;

static Eigen::Matrix<double, Eigen::Dynamic, 5> parseCSV(const string &filename);

int main(int argc, char **argv)
{
	boost::asio::io_service io;

	HydrophoneWorker hydWorker(io, 2, configPath);
	if(!hydWorker.Startup())
			throw new runtime_error("Failed to start HYD Worker!");

	DDSDomainParticipant *participant = DDSDomainParticipantFactory::get_instance()->create_participant(0, DDS_PARTICIPANT_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
	if (!participant)
		throw runtime_error("Failed to create DDSDomainParticipant");

	DDSDomainParticipant *participant1 = DDSDomainParticipantFactory::get_instance()->create_participant(0, DDS_PARTICIPANT_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
		if (!participant)
			throw runtime_error("Failed to create DDSDomainParticipant");

	if (HydrophoneMessageTypeSupport::register_type(participant, HydrophoneMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
			throw runtime_error("Failed to register type");

	if (HydrophoneMessageTypeSupport::register_type(participant1, HydrophoneMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
				throw runtime_error("Failed to register type");

	HydrophoneDDSCommander ddsCommander(hydWorker, participant1);

	HydrophoneDDSListener ddsListener(hydWorker, participant);

	io.run();

	hydWorker.Shutdown();

}

/*
int main(int argc, char **argv) {
	HydrophoneDataProcessor::Config config;
	config.load(configPath + string("/hydrophone.config"));

	Eigen::Matrix<double, Eigen::Dynamic, 5> data = parseCSV("Temp.csv");

	int start=0;
	while (start < data.rows()) {
		int stop = start;
		while (stop < data.rows() && (int)data(start, 0) == (int)data(stop, 0)) { stop++; }

		HydrophoneDataProcessor proc(data.block(start, 1, stop-start, 4), 27E3, config);
		cout << scientific << setprecision(16);
		cout << proc.getDist() << ", " << proc.getAngle1()/M_PI*180 << ", " << proc.getAngle2()/M_PI*180 << endl;

		start = stop;
	}
}*/
/*
int main(int argc, char **argv) {
	VectorXd vec(2048);

	for (int i=0; i<256; i++) {
		vec[i] = sin(2*M_PI*.1*i);
	}

	for (int i=256; i<vec.rows(); i++) {
		vec[i] = 0;
	}

	VectorXcd a;
	Eigen::FFT<double> fft;
	fft.fwd(a, vec);

	VectorXd a_abs = a.array().abs();
	int maxindex;
	a_abs.maxCoeff(&maxindex);

	cout << maxindex/2048.0*200000 << endl;
}*/

static Eigen::Matrix<double, Eigen::Dynamic, 5> parseCSV(const string &filename) {
	ifstream in(filename.c_str());
	vector<vector<double> > datavec;

	while (!in.eof()) {
		string str;
		getline(in, str);

		if (!str.size())
			continue;

		vector<double> vec;
		string::iterator pos = str.begin();
		while (true) {
			string::iterator next = find(pos, str.end(), ',');
			string num(pos, next);

			if (!num.size())
				continue;

			vec.push_back(lexical_cast<double>(num));

			if (next == str.end())
				break;
			pos = next+1;
		}

		vec.resize(5);
		datavec.push_back(vec);
	}

	Eigen::Matrix<double, Eigen::Dynamic, 5> data(datavec.size(), 5);

	for (unsigned int row=0; row<datavec.size(); row++) {
		for (unsigned int col=0; col<5; col++) {
			data(row, col) = datavec[row][col];
		}
	}

	return data;
}

