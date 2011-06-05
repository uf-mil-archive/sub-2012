#include "Hydrophone/HydrophoneDataProcessor.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <algorithm>
#include <sstream>

using namespace subjugator;
using namespace Eigen;
using namespace boost::property_tree;
using namespace boost::property_tree::xml_parser;
using namespace std;

HydrophoneDataProcessor::HydrophoneDataProcessor(const Data &rawdata, double pingfreq, const Config &config)
: data(rawdata), pingfreq(pingfreq), valid(false), template_pos(0) {
	period = (int)round((config.samplingrate * config.scalefact) / pingfreq); // compute period from ping freq

	processRawData(config);
	checkData(config);

	makeTemplate(config);
	computeDeltas(config);
	computeAngles(config);
}

void HydrophoneDataProcessor::processRawData(const Config &config) {
	// zero mean the data
	for (int i=0; i<4; i++)
		data.col(i).array() -= data.col(i).head(40).mean(); // the first 40 samples are always zero samples

	// filter the data
	for (int i=0; i<4; i++)
		data.col(i) = filter(config.bandpass_fircoefs, data.col(i));

	// scale the data
	for (int i=0; i<data.cols(); i++) {
		data.col(i) /= data.col(i).array().abs().maxCoeff();
	}

	// prepare upsamp extra large since we want to have zeros at the beginning
	int zerorows = 100*config.scalefact;
	data_upsamp = Data::Zero(data.rows()*config.scalefact + zerorows, 4);

	// upsample
	for (int i=0; i<4; i++)
		data_upsamp.col(i).segment(zerorows, data.rows()*config.scalefact) = upsample(data.col(i), config.scalefact, config.upsample_fircoefs);
}

void HydrophoneDataProcessor::checkData(const Config &config) {
	Data data_pad = Data::Zero(2048, 4);

	// multiply through by a hamming window
	for (int i=0; i<4; i++) {
		VectorXd temp = data.col(i).cwiseProduct(config.hamming_fircoefs);
		data_pad.block(0, i, temp.rows(), 1) = temp;
	}

	// take the FFT of the first column
	VectorXd datacol = data_pad.col(0);
	VectorXcd a;
	fft.fwd(a, datacol);

	// find the maximum location
	int max_loc;
	a.head(datacol.rows()/2).cwiseAbs().maxCoeff(&max_loc);

	// compute the frequency
	double max_freq = (config.samplingrate/2)/(datacol.rows()/2) * (max_loc+1);
	if (abs(max_freq - pingfreq) < config.freqthresh) // if its within the threshold
		valid = true; // data is valid
}

void HydrophoneDataProcessor::makeTemplate(const Config &config) {
	// determine the center of the template, which is the first non-zero sample
	template_pos=0;
	while (template_pos < data_upsamp.rows() && abs(data_upsamp(template_pos, 0)) < .1)
		template_pos++;

	// determine if the template goes past the beginning or the end of the data
	if (template_pos - 3*period < 0 || template_pos+3*period >= data_upsamp.rows())
		throw Error("Template positioned such that start or endpoints lie outside the data");
}

void HydrophoneDataProcessor::computeDeltas(const Config &config) {
	double dist_h = .9*2.54/100; // distance between hydrophones
	int arr_dist = (int)floor((dist_h/config.soundvelocity)*config.samplingrate*config.scalefact*.95); // range to look for a match
	int matchstart = template_pos + 3*period - arr_dist; // start position for match searching
	int matchstop = template_pos + 3*period + arr_dist; // stop position for match searching

	for (int i=0; i<3; i++) { // for the three other signals
		double matchpos = matchTemplate(i+1, matchstart, matchstop, config); // find a match
		deltas[i] = (matchpos - template_pos)/(config.samplingrate*config.scalefact)*config.soundvelocity; // compute the delta
	}
}

double HydrophoneDataProcessor::matchTemplate(int channel, int start, int stop, const Config &config) {
	int tlen = period*6+1;
	int template_start = template_pos - 3*period;

	// vector holding the mean absolute difference between the template and the target signal, with the template offset at each position
	VectorXd mad = VectorXd::Zero(data_upsamp.rows());

	// compute each element of mad, within the start and stop range
	for (int i=start; i<=stop; i++) {
		mad(i) = (data_upsamp.col(channel).segment(i - tlen + 1, tlen) - data_upsamp.col(0).segment(template_start, tlen)).array().abs().mean();
	}

	// compute the maximum value of mad
	double mad_max = mad.maxCoeff();
	mad.head(start).fill(mad_max); // fill areas of mad outside start and stop range with the maximum value
	mad.tail((mad.rows() - stop)).fill(mad_max);

	int aprox_min_pt;
	mad.minCoeff(&aprox_min_pt); // find the approximate minimum position

	VectorXd d_mad = filter(Vector3d(.5, 0, -.5), mad); // then take the derivative of mad
	double min_pt = findZeros(d_mad, aprox_min_pt - 2); // and use it to find an interpolated minimum position

	double delta = min_pt-1-(tlen-1)/2.0; // compute the delta
	return delta;
}

void HydrophoneDataProcessor::computeAngles(const Config &config) {
	double y1 = deltas[0];
	double y2 = deltas[1];
	double y3 = deltas[2];

	double dist_h = config.disth;
	double dist_h4 = config.disth4;

	double dist = abs((y1*y1 + y2*y2 - 2*dist_h*dist_h)/(2*y1 + 2*y2));

	double cos_alpha1 = (2*dist*y1 + y1*y1 - dist_h*dist_h)/(-2*dist*dist_h);
	double cos_alpha2 = -(2*dist*y2 + y2*y2 - dist_h*dist_h)/(-2*dist*dist_h);
	double cos_alpha = (cos_alpha1 + cos_alpha2)/2;
	double alpha = acos(cos_alpha);

	double cos_beta = (2*dist*y3 + y3*y3 - dist_h4*dist_h4)/(-2*dist*dist_h4);
	double beta = acos(cos_beta);

	double dist_x = cos_alpha*dist;
	double dist_y = cos_beta*dist;
	double dist_z;
	if (dist*dist - (dist_x*dist_x + dist_y*dist_y) < 0)
		dist_z = 0;
	else
		dist_z = sqrt(dist*dist - (dist_x*dist_x + dist_y*dist_y));

	ang1 = atan2(dist_y, dist_x);
	ang2 = atan2(dist_z, sqrt(dist_x*dist_x + dist_y*dist_y));
	sph_dist = sqrt(dist_x*dist_x + dist_y*dist_y + dist_z*dist_z);
}

static int sgn(double x) {
	if (x > 0)
		return 1;
	else if (x < 0)
		return -1;
	else
		return 0;
}

double HydrophoneDataProcessor::findZeros(const Eigen::VectorXd &data, int start) {
	for (int i=start; i<data.rows()-1; i++) {
		if (sgn(data[i]) != sgn(data[i+1])) {
			double y2 = data(i+1);
			double y1 = data(i);
			double x2 = i+1;
			double x1 = i;
			return -(x2-x1)/(y2-y1)*y1+x1;
		}
	}

	return 0;
}

VectorXd HydrophoneDataProcessor::filter(const VectorXd &coefs, const VectorXd &data) {
	VectorXd out(data.rows());

	for (int i=0; i<data.rows(); i++) {
		int len = min(i+1, (int)coefs.rows());

		// compute each element as a dot product of blocks of the two vectors, which should get optimized nicely
		out[i] = coefs.head(len).reverse().dot(data.segment(i-len+1, len));
	}

	return out;
}

VectorXd HydrophoneDataProcessor::upsample(const VectorXd &in, int p, const VectorXd &filter_coefs) {
	VectorXd inzeroed((in.rows()-1)*p + filter_coefs.size());

	// insert zeros inbetween the data
	for (int i=0; i<in.rows()*p; i++) {
		if (i % p == 0)
			inzeroed[i] = in[i/p];
		else
			inzeroed[i] = 0;
	}
	inzeroed.tail(inzeroed.size() - in.rows()*p).fill(0); // fill the left-over spaces with zeros

	VectorXd out = filter(filter_coefs, inzeroed); // run the lowpass filter

	int delay = (int)ceil((filter_coefs.rows()-1)/2); // compute the delay of the filter
	return out.segment(delay, in.rows()*p); // and return the correct amount of data taking the delay into account
}

void HydrophoneDataProcessor::Config::load(const string &filename) {
	ptree pt;
	read_xml(filename, pt);

	scalefact = pt.get<int>("scalefact");
	samplingrate = pt.get<int>("samplingrate");
	soundvelocity = pt.get<double>("soundvelocity");
	freqthresh = pt.get<double>("freqthresh");
	disth = pt.get<double>("disth");
	disth4 = pt.get<double>("disth4");
	bandpass_fircoefs = strToVec(pt.get<string>("bandpass"));
	upsample_fircoefs = strToVec(pt.get<string>("upsample"));
	hamming_fircoefs = strToVec(pt.get<string>("hamming"));
}

// kind of stumped on a better way to make this work
Eigen::VectorXd HydrophoneDataProcessor::Config::strToVec(const std::string &str) {
	vector<double> vals; // using a STL vector to get power of 2 resizing
	stringstream ss(str);
	while (true) {
		double val;
		ss >> val;
		if (ss.eof())
			break;
		vals.push_back(val);
	}

	VectorXd vec(vals.size()); // no good way to go STL -> Eigen?
	for (int i=0; i < vals.size(); i++)
		vec[i] = vals[i];

	return vec;
}


