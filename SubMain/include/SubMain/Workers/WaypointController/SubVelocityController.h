#ifndef SUBVELOCITYCONTROLLER_H
#define SUBVELOCITYCONTROLLER_H

#include "SubMain/Workers/LPOSVSS/SubAttitudeHelpers.h"
#include <Eigen/Dense>
#include <cmath>

using namespace Eigen;

namespace subjugator
{
	class VelocityController
	{
	public:
		typedef Matrix<double, 6, 1> Vector6d;
		typedef Matrix<double, 6, 6> Matrix6d;
	public:
		VelocityController(Vector6d k, Vector6d ks, Vector6d alpha, Vector6d beta);

		Vector6d& GetWrench() const( return currentControl; )
		void Update(boost::int16_t currentTick, const TrajectoryData& traj, const LPOSVSSInfo& lposInfo)

	private:
		static const double SECPERNANOSEC = 1e-9;

		Matrix6d k;
		Matrix6d ks;
		Matrix6d ksPlus1;
		Matrix6d alpha;
		Matrix6d beta;

		Vector6d e;
		Vector6d e2;
		Vector6d r;
		Vector6d rise_term;
		Vector6d rise_term_int;
		Vector6d rise_term_prev;
		Vector6d rise_term_int_prev;

		Matrix6d J;
		Matrix6d J_inv;

		Vector6d x;
		Vector6d x_dot;

		Vector6d vb;

		Vector6d xd;
		Vector6d xd_dot;

		boost::int64_t previousTime;

		Vector6d currentControl;

		void UpdateJacobian(const Vector6d& x);
		void UpdateJacobianInverse(const Vector6d& x);
		Vector6d RiseFeedbackNoAccel(double dt);
		Vector6d PDFeedback(double dt);
	};
}



#endif /* SUBVELOCITYCONTROLLER_H */
