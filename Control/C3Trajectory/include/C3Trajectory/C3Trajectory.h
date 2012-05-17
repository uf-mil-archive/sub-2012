#ifndef C3TRAJECTORY_C3FILTER_H
#define C3TRAJECTORY_C3FILTER_H

#include "LibSub/Math/EigenUtils.h"

namespace subjugator {
	class C3Trajectory {
		public:
		struct Limits {
			Vector6d vmin_b;
			Vector6d vmax_b;
			Vector6d amin_b;
			Vector6d amax_b;
			Vector6d umax_b;
		};

		C3Trajectory(const Vector6d &qinit, const Vector6d &qdotinit, const Limits &limits);
		inline void setLimits(const Limits &limits) { this->limits = limits; }

		void update(double dt, const Vector6d &r);

		struct Point {
			Vector6d q;
			Vector6d qdot;
		};

		Point getCurrentPoint() const;

	private:
		Vector6d q;
		Vector6d qdot;
		Vector6d qdotdot_b;
		Vector6d u_b;

		Limits limits;

		static double c3filter(double q, double qdot, double qdotdot,
		                       double r, double rdot, double rdotdot,
		                       double vmin, double vmax,
		                       double amin, double amax,
		                       double umax);
		static Matrix6d jacobian(const Vector3d &rpy);
		static std::pair<Vector3d, Vector3d> limit(const Vector3d &vmin, const Vector3d &vmax, const Vector3d &delta);
	};
};

#endif
