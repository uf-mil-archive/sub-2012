#ifndef CONTROLLERGAINS_H
#define CONTROLLERGAINS_H

#include "HAL/format/DataObject.h"

#include <Eigen/Dense>

using namespace Eigen;

namespace subjugator {
	class ControllerGains : public DataObject {
		public:
		ControllerGains(){}
		ControllerGains(Matrix<double, 6, 1> k, Matrix<double, 6, 1> ks, Matrix<double, 6, 1> alpha, Matrix<double, 6, 1> beta)	:
			k(k), ks(ks), alpha(alpha), beta(beta){}

			Matrix<double, 6, 1> k;
			Matrix<double, 6, 1> ks;
			Matrix<double, 6, 1> alpha;
			Matrix<double, 6, 1> beta;
	};
}

#endif /* CONTROLLERGAINS_H */
