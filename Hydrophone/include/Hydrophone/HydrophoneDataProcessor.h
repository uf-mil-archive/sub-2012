#ifndef HYDROPHONEDATAPROCESSOR_H
#define HYDROPHONEDATAPROCESSOR_H

#include <Eigen/Dense>
#include <unsupported/Eigen/FFT>
#include <stdexcept>
#include <string>

class HydrophoneDataProcessor {
	public:
		typedef Eigen::Matrix<double, Eigen::Dynamic, 4> Data;

		struct Config {
			int scalefact; // the factor by which the raw data is scaled up for processing
			int samplingrate; // the rate at which the raw data was collected
			double pingfreq;
			double soundvelocity;
			double freqthresh;

			Eigen::VectorXd bandpass_fircoefs;
			Eigen::VectorXd upsample_fircoefs;
			Eigen::VectorXd hamming_fircoefs;

			void load(const std::string &filename);

			private:
				static Eigen::VectorXd strToVec(const std::string &str);
		};

		HydrophoneDataProcessor(const Data &rawdata, const Config &config);

		struct Error : public std::runtime_error {
			inline Error(const std::string &what) : runtime_error(what) { }
		};

		inline double getAngle1() const { return ang1; }
		inline double getAngle2() const { return ang2; }
		inline double getDist() const { return sph_dist; }
		inline bool isValid() const { return valid; }

	private:
		void processRawData(const Config &config);
		void checkData(const Config &config);
		void makeTemplate(const Config &config);
		void computeDeltas(const Config &config);
		void computeAngles(const Config &config);

		double matchTemplate(int channel, int start, int stop, const Config &config);
		double findZeros(const Eigen::VectorXd &data, int start);

		Data data;
		Data data_upsamp;
		int template_pos; // center of template
		double deltas[3];
		bool valid;

		double ang1, ang2;
		double sph_dist;

		Eigen::FFT<double> fft;

	public:
		// helpers
		static Eigen::VectorXd filter(const Eigen::VectorXd &coefs, const Eigen::VectorXd &data);
		static Eigen::VectorXd upsample(const Eigen::VectorXd &in, int p, const Eigen::VectorXd &coefs);
};

#endif

