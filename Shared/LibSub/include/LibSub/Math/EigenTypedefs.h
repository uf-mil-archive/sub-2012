#ifndef LIBSUB_MATH_EIGENTYPEDEFS
#define LIBSUB_MATH_EIGENTYPEDEFS

#include <Eigen/Dense>

namespace subjugator {
	// Common
	typedef Matrix<double, 6, 1> Vector6d;

	// Controller
	typedef Matrix<double, 6, 6> Matrix6d;
	typedef Matrix<double, 19, 1> Vector19d;
	typedef Matrix<double, 19, 19> Matrix19d;
	typedef Matrix<double, 19, 5> Matrix19x5d;

	// LPOSVSS
	typedef Matrix<double, 13, 1> Vector13d;
	typedef Matrix<double, 1, 26> RowVector26d;
	typedef Matrix<double, 1, 27> RowVector27d;
	typedef Matrix<double, 13, 13> Matrix13d;
	typedef Matrix<double, 7, 7> Matrix7d;
	typedef Matrix<double, 7, 1> Vector7d;
	typedef Matrix<double, 13, 7> Matrix13x7d;
	typedef Matrix<double, 13, 12> Matrix13x12d;
	typedef Matrix<double, 13, 26> Matrix13x26d;
	typedef Matrix<double, 13, 27> Matrix13x27d;
	typedef Matrix<double, 12, 13> Matrix12x13d;
	typedef Matrix<double, 12, 12> Matrix12d;
	typedef Matrix<double, 4, 27> Matrix4x27d;
	typedef Matrix<double, 3,27> Matrix3x27d;
	typedef Matrix<double, 7, 27> Matrix7x27d;
	typedef Matrix<double, 7, 26> Matrix7x26d;
}

#endif

