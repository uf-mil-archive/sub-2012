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

		struct Point {
			Vector6d q;
			Vector6d qdot;

			Point() { }

			Point(const Vector6d &q, const Vector6d &qdot) :
				q(q), qdot(qdot) { }
		};

		C3Trajectory(const Point &start, const Limits &limits);
		inline void setLimits(const Limits &limits) { this->limits = limits; }

		void update(double dt, const Vector6d &r);


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
		static Matrix4d transformation(const Vector6d &q);
		static Matrix4d inverse_transformation(const Vector6d &q);
		static std::pair<Vector3d, Vector3d> limit(const Vector3d &vmin, const Vector3d &vmax, const Vector3d &delta);
	};
};

#endif
