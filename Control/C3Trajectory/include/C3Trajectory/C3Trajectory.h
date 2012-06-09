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
			Vector3d arevoffset_b;
			Vector6d umax_b;
		};

		struct Point {
			Vector6d q;
			Vector6d qdot;

			Point() { }

			Point(const Vector6d &q, const Vector6d &qdot) :
				q(q), qdot(qdot) { }
		};

		struct Waypoint {
			Point r;
			double speed;
			bool coordinate_unaligned;

			Waypoint() { }
		Waypoint(const Point &r) : r(r), speed(0), coordinate_unaligned(true) { }
		};

		C3Trajectory(const Point &start, const Limits &limits);
		void update(double dt, const Waypoint &waypoint, double waypoint_t);

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

		static std::pair<Matrix4d, Matrix4d> transformation_pair(const Vector6d &q);
		static std::pair<Vector3d, Vector3d> limit(const Vector3d &vmin, const Vector3d &vmax, const Vector3d &delta);
	};
};

#endif
