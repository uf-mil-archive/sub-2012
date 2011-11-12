#ifndef DATAOBJECTS_PDWRENCH_H
#define DATAOBJECTS_PDWRENCH_H

#include "HAL/format/DataObject.h"
#include <Eigen/Dense>
#include <boost/cstdint.hpp>
#include <cassert>

namespace subjugator {
	class PDWrench : public DataObject {
		public:
			typedef Eigen::Matrix<double, 6, 1> Vector6D;

			PDWrench(const Vector6D &vec) : vec(vec) { }

			inline const Vector6D &getVec() const { return vec; }

		private:
			Vector6D vec;
	};
}

#endif

