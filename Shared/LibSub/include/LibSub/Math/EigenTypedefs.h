#ifndef LIBSUB_MATH_EIGENTYPEDEFS
#define LIBSUB_MATH_EIGENTYPEDEFS

#include <Eigen/Dense>

namespace subjugator {
	// Common
	typedef Eigen::Matrix<double, 6, 1> Vector6d;

	// Controller
	typedef Eigen::Matrix<double, 6, 6> Matrix6d;
	typedef Eigen::Matrix<double, 19, 1> Vector19d;
	typedef Eigen::Matrix<double, 19, 19> Matrix19d;
	typedef Eigen::Matrix<double, 19, 5> Matrix19x5d;

	// LPOSVSS
	typedef Eigen::Matrix<double, 13, 1> Vector13d;
	typedef Eigen::Matrix<double, 1, 26> RowVector26d;
	typedef Eigen::Matrix<double, 1, 27> RowVector27d;
	typedef Eigen::Matrix<double, 13, 13> Matrix13d;
	typedef Eigen::Matrix<double, 7, 7> Matrix7d;
	typedef Eigen::Matrix<double, 7, 1> Vector7d;
	typedef Eigen::Matrix<double, 13, 7> Matrix13x7d;
	typedef Eigen::Matrix<double, 13, 12> Matrix13x12d;
	typedef Eigen::Matrix<double, 13, 26> Matrix13x26d;
	typedef Eigen::Matrix<double, 13, 27> Matrix13x27d;
	typedef Eigen::Matrix<double, 12, 13> Matrix12x13d;
	typedef Eigen::Matrix<double, 12, 12> Matrix12d;
	typedef Eigen::Matrix<double, 4, 27> Matrix4x27d;
	typedef Eigen::Matrix<double, 3,27> Matrix3x27d;
	typedef Eigen::Matrix<double, 7, 27> Matrix7x27d;
	typedef Eigen::Matrix<double, 7, 26> Matrix7x26d;
}

#endif

