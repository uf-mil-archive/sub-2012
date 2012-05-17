#include "C3Trajectory/C3Trajectory.h"
#include "boost/tuple/tuple.hpp"

#include <iostream>

using namespace subjugator;
using namespace Eigen;
using namespace boost;
using namespace std;

C3Trajectory::C3Trajectory(const Vector6d &qinit, const Vector6d &qdotinit, const Limits &limits) :
	q(qinit),
	qdot(qdotinit),
	qdotdot_b(Vector6d::Zero()),
	u_b(Vector6d::Zero()),
	limits(limits) { }

C3Trajectory::Point C3Trajectory::getCurrentPoint() const {
	Point p;
	p.q = q;
	p.qdot = qdot;
   	return p;
}

void C3Trajectory::update(double dt, const Vector6d &r) {
	Matrix6d J = jacobian(q.tail(3));
	Vector6d q_b = J*q;
	Vector6d r_b = J*r;
	r_b.head(3) -= q_b.head(3);
	q_b.head(3).fill(0);
	Vector6d qdot_b = J*qdot;

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
	Vector6d qdotdot = J.lu().solve(qdotdot_b);
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

Matrix6d C3Trajectory::jacobian(const Vector3d &rpy) {
	double sphi = sin(rpy(0));
	double cphi = cos(rpy(0));

	double stheta = sin(rpy(1));
	double ctheta = cos(rpy(1));
	double tantheta = tan(rpy(1));

	double spsi = sin(rpy(2));
	double cpsi = cos(rpy(2));

	Matrix6d j;
	j.block<3,3>(0, 0) <<
		cpsi*ctheta, -spsi*cphi + cpsi*stheta*sphi, spsi*sphi + cphi*cphi*stheta,
		spsi*ctheta, cpsi*cphi + sphi*stheta*spsi, -cpsi*sphi + stheta*spsi*cphi,
		-stheta, ctheta*sphi, ctheta*cphi;
	j.block<3,3>(0, 3).fill(0);
	j.block<3,3>(3, 0).fill(0);
	j.block<3,3>(3, 3) <<
		1, sphi*tantheta, cphi*tantheta,
		0, cphi, -sphi,
		0, sphi/ctheta, cphi/ctheta;
	return j;
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
