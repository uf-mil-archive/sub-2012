#include "TrackingController/TrackingController.h"
#include "LibSub/Math/AttitudeHelpers.h"
#include "LibSub/Math/Quaternion.h"
#include <boost/algorithm/string.hpp>

using namespace subjugator;
using namespace boost::algorithm;
using namespace Eigen;
using namespace std;

TrackingController::TrackingController(const Config &config) :
config(config),
rise_term_prev(Vector6d::Zero()),
rise_term_int_prev(Vector6d::Zero()),
V_hat_dot_prev(Matrix19x5d::Zero()),
V_hat_prev(Matrix19x5d::Random()),
W_hat_dot_prev(Matrix6d::Zero()),
W_hat_prev(Matrix6d::Zero()) { }

TrackingController::Output TrackingController::update(double dt, const TrajectoryPoint &t, const State &s) {
	// calculate some shared constants
	Matrix6d J_inv = jacobianInverse(s.x);

	Vector6d e;
	e << t.xd.block<3,1>(0,0) - s.x.block<3,1>(0,0),
	     AttitudeHelpers::DAngleDiff(s.x(3), t.xd(3)),
	     AttitudeHelpers::DAngleDiff(s.x(4), t.xd(4)),
	     AttitudeHelpers::DAngleDiff(s.x(5), t.xd(5));

	Vector6d vbd = J_inv * (config.gains.k.asDiagonal() * e + t.xd_dot);
	Vector6d e2 = vbd - s.vb;

	// compute each term
	Output out;
	out.control_pd = pdFeedback(dt, e2);
	out.control_rise = riseFeedbackNoAccel(dt, e2);
	out.control_nn = nnFeedForward(dt, e2, t);

	// sum the active terms to get the combined output
	out.control = Vector6d::Zero();
	if (config.mode & TERM_PD)
		out.control += out.control_pd;
	if (config.mode & TERM_RISE)
		out.control += out.control_rise;
	if (config.mode & TERM_NN)
		out.control += out.control_nn;

	return out;
}

Vector6d TrackingController::riseFeedbackNoAccel(double dt, const Vector6d &e2) {
	const Gains &g = config.gains;

	Matrix6d ksPlus1 = (Matrix6d)g.ks.asDiagonal() + Matrix6d::Identity();

	Vector6d rise_term_int = ksPlus1*g.alpha.asDiagonal()*e2 + g.beta.asDiagonal()*signs(e2);

	Vector6d rise_term = rise_term_prev + dt / 2.0 * (rise_term_int + rise_term_int_prev);

	Vector6d rise_control = ksPlus1 * e2 + rise_term;

	// Save previous values
	rise_term_prev = rise_term;
	rise_term_int_prev = rise_term_int;

	return rise_control;
}

Vector6d TrackingController::pdFeedback(double dt, const Vector6d &e2) {
	return config.gains.ks.asDiagonal() * e2;
}

Vector6d TrackingController::nnFeedForward(double dt, const Vector6d &e2, const TrajectoryPoint &t) {
	const Gains &g = config.gains;

	Vector6d xd_dotdot = (t.xd_dot - xd_dot_prev) / dt;
	xd_dot_prev = t.xd_dot;
	Vector6d xd_dotdotdot = (xd_dotdot - xd_dotdot_prev) / dt;
	xd_dotdot_prev = xd_dotdot;

	VectorXd xd_nn(t.xd.rows()*3+1, 1); // xd_nn = [1 ; xd; xd_dot; xd_dotdot];
	xd_nn << 1, t.xd, t.xd_dot, xd_dotdot;

	VectorXd xd_nn_dot(t.xd.rows()*3+1, 1); // xd_nn_dot = [0 ; xd_dot; xd_dotdot; xd_dotdotdot];
	xd_nn_dot << 0, t.xd_dot, xd_dotdot, xd_dotdotdot;

	VectorXd one = VectorXd::Ones(V_hat_prev.cols(), 1); // not sure why I need the matrix form of ::Ones here, the vector form hits a static assert
	VectorXd sigma = one.cwiseQuotient(one + (-V_hat_prev.transpose() * xd_nn).array().exp().matrix());

	VectorXd sigma_hat(1+sigma.rows(),1); //sigma_hat = [1; sigma];
	sigma_hat << 1, sigma;

	MatrixXd sigma_hat_prime_term = sigma.asDiagonal() * (MatrixXd::Identity(sigma.rows(), sigma.rows()) - (MatrixXd)sigma.asDiagonal());
	MatrixXd sigma_hat_prime(1+sigma_hat_prime_term.rows(), sigma_hat_prime_term.cols());
	sigma_hat_prime.fill(0.0);
	sigma_hat_prime.block(1, 0, sigma_hat_prime_term.rows(), sigma_hat_prime_term.cols()) = sigma_hat_prime_term;

	Matrix6d W_hat_dot = g.gamma1.asDiagonal() * sigma_hat_prime * V_hat_prev.transpose() * xd_nn_dot * e2.transpose();
	Matrix19x5d V_hat_dot = g.gamma2.asDiagonal() * xd_nn_dot * (sigma_hat_prime.transpose() * W_hat_prev * e2).transpose();

	// integrate W_hat_dot and V_hat_dot
	Matrix6d W_hat = W_hat_prev + dt / 2.0 * (W_hat_dot + W_hat_dot_prev);
	Matrix19x5d V_hat = V_hat_prev + dt / 2.0 * (V_hat_dot + V_hat_dot_prev);

	Vector6d nn_control = W_hat.transpose() * sigma_hat;

	// save previous values
	W_hat_prev = W_hat;
	W_hat_dot_prev = W_hat_dot;
	V_hat_prev = V_hat;
	V_hat_dot_prev = V_hat_dot;

/*	cout << "xd_nn_dot=======" << endl << xd_nn_dot << endl;
	cout << "V_hat_dot=======" << endl << V_hat_dot << endl;
	cout << "W_hat_dot=======" << endl << W_hat_dot << endl;
	cout << "gamma1==========" << endl << gamma1 << endl;
	cout << "V_hat===========" << endl << V_hat << endl;
	cout << "W_hat===========" << endl << W_hat << endl;*/

	return nn_control;
}

Matrix6d TrackingController::jacobian(const Vector6d& x) {
	double sphi = sin(x(3));
	double cphi = cos(x(3));

	double stheta = sin(x(4));
	double ctheta = cos(x(4));
	double tantheta = tan(x(4));

	double spsi = sin(x(5));
	double cpsi = cos(x(5));

	Matrix6d J = Matrix6d::Zero();
	J.block<3,3>(0,0) << cpsi * ctheta,
	                     -spsi * cphi + cpsi * stheta * sphi,
	                     spsi * sphi + cpsi * cphi * stheta,
	                     spsi * ctheta,
	                     cpsi * cphi + sphi * stheta * spsi,
	                     -cpsi * sphi + stheta * spsi * cphi,
	                     -stheta,
	                     ctheta * sphi,
	                     ctheta * cphi;
	J.block<3,3>(3,3) << 1,
	                     sphi * tantheta,
	                     cphi * tantheta,
	                     0,
	                     cphi,
	                     -sphi,
	                     0,
	                     sphi / ctheta,
	                     cphi / ctheta;
	return J;
}

Matrix6d TrackingController::jacobianInverse(const Vector6d& x) {
	double sphi = sin(x(3));
	double cphi = cos(x(3));

	double stheta = sin(x(4));
	double ctheta = cos(x(4));

	double spsi = sin(x(5));
	double cpsi = cos(x(5));

	Matrix6d J_inv = Matrix6d::Zero();
	J_inv.block<3,3>(0,0) << cpsi * ctheta,
	                         spsi * ctheta,
	                         -stheta,
	                         -spsi * cphi + cpsi * stheta * sphi,
	                         cpsi * cphi + sphi * stheta * spsi,
	                         ctheta * sphi,
	                         spsi * sphi + cpsi * cphi * stheta,
	                         -cpsi * sphi + stheta * spsi * cphi,
	                         ctheta * cphi;

	J_inv.block<3,3>(3,3) << 1,
	                         0,
	                         -stheta,
	                         0,
	                         cphi,
	                         ctheta * sphi,
	                         0,
	                         -sphi,
	                         ctheta*cphi;
	return J_inv;
}

istream &subjugator::operator>>(istream &in, TrackingController::Mode &mode) {
	string str;
	in >> str;

	if (iequals(str, "pd"))
		mode = TrackingController::MODE_PD;
	else if (iequals(str, "rise"))
		mode = TrackingController::MODE_RISE;
	else if (iequals(str, "rise_nn"))
		mode = TrackingController::MODE_RISE_NN;
	else
		in.setstate(istream::badbit);

	return in;
}

