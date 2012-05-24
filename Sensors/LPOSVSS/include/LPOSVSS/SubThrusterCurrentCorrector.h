#ifndef SUBTHRUSTERCURRENTCORRECTOR_H
#define SUBTHRUSTERCURRENTCORRECTOR_H

#include <Eigen/Dense>
#include <vector>
#include <boost/array.hpp>

namespace subjugator {
	class ThrusterCurrentCorrector {
	public:
		struct Config {
			boost::array<Eigen::Vector3d, 4> forward;
			boost::array<Eigen::Vector3d, 4> reverse;
		};

		ThrusterCurrentCorrector(const Config &config);

		Eigen::Vector3d calculate(double current) const;

		static Eigen::Vector3d CalculateTotalCorrection(const std::vector<ThrusterCurrentCorrector>& tList, const std::vector<double>& currents);
	private:
		Config conf;
	};
}


#endif /* SUBTHRUSTERCURRENTCORRECTOR_H */
