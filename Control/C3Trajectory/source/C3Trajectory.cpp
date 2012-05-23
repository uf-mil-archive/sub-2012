#include "C3Trajectory/C3Trajectory.h"
#include "LibSub/Math/AttitudeHelpers.h"
#include "boost/tuple/tuple.hpp"

using namespace subjugator;
using namespace Eigen;
using namespace boost;
using namespace std;

C3Trajectory::C3Trajectory(const Point &start, const Limits &limits) :
	q(start.q),
	qdot(start.qdot),
	qdotdot_b(Vector6d::Zero()),
	u_b(Vector6d::Zero()),
	limits(limits) { }

C3Trajectory::Point C3Trajectory::getCurrentPoint() const {
	Point p;
	p.q = q;
	p.qdot = qdot;
   	return p;
}

static Vector6d apply(const Matrix4d &T, const Vector6d &q, double w) {
	Vector6d q_t;
	q_t << q.head(3), w;
	q_t.head(4) = T * q_t.head(4);
	q_t.tail(3) = q.tail(3);
	return q_t;
}

void C3Trajectory::update(double dt, const Vector6d &r) {
	pair<Matrix4d, Matrix4d> Ts = transformation_pair(q);
	const Matrix4d &T = Ts.first;
	const Matrix4d &T_inv = Ts.second;

	Vector6d q_b = apply(T, q, 1);
	Vector6d r_b = apply(T, r, 1);
	Vector6d qdot_b = apply(T, qdot, 0);

	Vector6d vmin_b_prime = limits.vmin_b;
	Vector6d vmax_b_prime = limits.vmax_b;
	Vector3d posdelta = r_b.head(3);
	if (posdelta.norm() > 0.1) {
		pair<Vector3d, Vector3d> result = limit(limits.vmin_b.head(3), limits.vmax_b.head(3), posdelta);
		vmin_b_prime.head(3) = result.first;
		vmax_b_prime.head(3) = result.second;
	}

	for (int i=0; i<6; i++) {
		u_b(i) = c3filter(q_b(i), qdot_b(i), qdotdot_b(i), r_b(i), 0, 0, vmin_b_prime(i), vmax_b_prime(i), limits.amin_b(i), limits.amax_b(i), limits.umax_b(i));
	}

	qdotdot_b += dt*u_b;
	Vector6d qdotdot = apply(T_inv, qdotdot_b, 0);
	qdot += dt*qdotdot;
	q += dt*qdot;
}

static double deltav(double v, double edot, double edotdot) {
	return edotdot*abs(edotdot) + 2*(edot - v);
}

static double ucv(double v, double edot, double edotdot, double umax) {
	double tmp = deltav(v, edot, edotdot);
	return -umax*sign(tmp + (1 - abs(sign(tmp)))*edotdot);
}

static double ua(double a, double edotdot, double umax) {
	return -umax * sign(edotdot - a);
}

static double uv(double v, double edot, double edotdot,
                 double edotdotmin, double edotdotmax, double umax) {
	return max(ua(edotdotmin, edotdot, umax),
	           min(ucv(v, edot, edotdot, umax),
	               ua(edotdotmax, edotdot, umax)));
}

double C3Trajectory::c3filter(double q, double qdot, double qdotdot,
                              double r, double rdot, double rdotdot,
                              double vmin, double vmax,
                              double amin, double amax,
                              double umax) {
	double e = (q - r) / umax;
	double edot = (qdot - rdot) / umax;
	double edotdot = (qdotdot - rdotdot) / umax;

	double edotmin = (vmin - rdot) / umax;
	double edotmax = (vmax - rdot) / umax;
	double edotdotmin = (amin - rdotdot) / umax;
	double edotdotmax = (amax - rdotdot) / umax;

	double delta = edot + edotdot * abs(edotdot)/2;
	double sd = sign(delta);

	double S;
	if (edotdotmax != 0 && edotdot <= edotdotmax && edot <= pow(edotdot, 2)/2 - pow(edotdotmax, 2)) {
		S = e - edotdotmax*(pow(edotdot,2) - 2*edot)/4 - pow(pow(edotdot,2) - 2*edot,2)/(8*edotdotmax) - edotdot*(3*edot - pow(edotdot,2))/3;
	} else if (edotdotmin != 0 && edotdot >= edotdotmin && edot >= pow(edotdotmin,2) - pow(edotdot,2)/2) {
		S = e - edotdotmin*(pow(edotdot,2) + 2*edot)/4 - pow(pow(edotdot,2) + 2*edot,2)/(8*edotdotmin) + edotdot*(3*edot + pow(edotdot,2))/3;
	} else {
		S = e + edot*edotdot*sd - (pow(edotdot,3))/6*(1-3*abs(sd)) + sd/4*sqrt(2*pow(pow(edotdot,2) + 2*edot*sd,3));
	}

	double uc = -umax*sign(S + (1-abs(sign(S)))*(delta + (1 - abs(sd))*edotdot));
	double uv_emin = uv(edotmin, edot, edotdot, edotdotmin, edotdotmax, umax);
	double uv_emax = uv(edotmax, edot, edotdot, edotdotmin, edotdotmax, umax);

	return max(uv_emin, min(uc, uv_emax));
}

pair<Matrix4d, Matrix4d> C3Trajectory::transformation_pair(const Vector6d &q) {
	Matrix4d R;
	R.block<3,3>(0, 0) = AttitudeHelpers::EulerToRotation(q.tail(3));
	R.block<1,3>(3, 0).fill(0);
	R.block<3,1>(0, 3).fill(0);
	R(3, 3) = 1;

	Matrix4d T = Matrix4d::Identity();
	T.block<3,1>(0, 3) = q.head(3);

	pair<Matrix4d, Matrix4d> result;
	result.first = R*T; // NED -> BODY

	T.block<3,1>(0, 3) = -q.head(3);
	result.second = T*R.transpose(); // BODY -> NED

	return result;
}

std::pair<Vector3d, Vector3d> C3Trajectory::limit(const Vector3d &vmin, const Vector3d &vmax, const Vector3d &delta) {
	Vector3d adelta = delta.array().abs();

	double maxtime = 0;
	for (int i=0; i<3; i++) {
		double time;
		if (delta(i) > 0) {
			time = adelta(i) / vmax(i);
		} else {
			time = adelta(i) / -vmin(i);
		}

		maxtime = max(time, maxtime);
	}

	assert(maxtime > 0);

	Vector3d av_prime = adelta / maxtime;
	Vector3d maxv_prime;
	Vector3d minv_prime;

	for (int i=0; i<3; i++) {
		if (delta(i) > 0.001) {
			maxv_prime(i) = av_prime(i);
			minv_prime(i) = vmin(i);
		} else if (delta(i) < -0.001) {
			maxv_prime(i) = vmax(i);
			minv_prime(i) = -av_prime(i);
		} else {
			maxv_prime(i) = 0;
			minv_prime(i) = 0;
		}
	}

	return make_pair(minv_prime, maxv_prime);
}
