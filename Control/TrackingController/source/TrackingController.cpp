#include "TrackingController/TrackingController.h"
#include "SubMain/SubAttitudeHelpers.h"
#include "SubMain/SubMILQuaternion.h"
#include "SubMain/SubPrerequisites.h"
#include <boost/algorithm/string.hpp>

using namespace subjugator;
using namespace boost::algorithm;
using namespace Eigen;
using namespace std;

static Matrix6d GetJacobian(const Vector6d& x);
static Matrix6d GetJacobianInverse(const Vector6d& x);
static Vector6d GetSigns(const Vector6d& x);

TrackingController::TrackingController(const Config &config) :
config(config),
rise_term_prev(Vector6d::Zero()),
rise_term_int_prev(Vector6d::Zero()),
V_hat_dot_prev(Matrix19x5d::Zero()),
V_hat_prev(Matrix19x5d::Random()),
W_hat_dot_prev(Matrix6d::Zero()),
W_hat_prev(Matrix6d::Zero()) { }

void TrackingController::update(double dt, const TrajectoryInfo& traj, const LPOSVSSInfo& lposInfo, TrackingControllerInfo &info) {
	// NED Position
	Vector6d x;
	x << lposInfo.getPosition_NED(),
	     MILQuaternionOps::Quat2Euler(lposInfo.getQuat_NED_B());

	// NED Velocity
	Vector6d x_dot;
	x_dot << lposInfo.getVelocity_NED(),
	         MILQuaternionOps::QuatRotate(lposInfo.getQuat_NED_B(), lposInfo.getAngularRate_BODY());

	// Body Velocity
	Vector6d vb;
	vb << MILQuaternionOps::QuatRotate(MILQuaternionOps::QuatInverse(lposInfo.getQuat_NED_B()), lposInfo.getVelocity_NED()),
	      lposInfo.getAngularRate_BODY();

	// Save the relevant trajectory data
	Vector6d xd = traj.getTrajectory();
	Vector6d xd_dot = traj.getTrajectory_dot();

	Matrix6d J_inv = GetJacobianInverse(x);
	//Matrix6d J = GetJacobian(x);

	Vector6d e;
	e << xd.block<3,1>(0,0) - x.block<3,1>(0,0),
	     AttitudeHelpers::DAngleDiff(x(3), xd(3)),
	     AttitudeHelpers::DAngleDiff(x(4), xd(4)),
	     AttitudeHelpers::DAngleDiff(x(5), xd(5));

	Vector6d vbd = J_inv * (config.gains.k.asDiagonal() * e + xd_dot);
	Vector6d e2 = vbd - vb;

	Vector6d control = Vector6d::Zero();
	Vector6d pd_control = PDFeedback(dt, e2);
	Vector6d rise_control = RiseFeedbackNoAccel(dt, e2);
	Vector6d nn_control = NNFeedForward(dt, e2, xd, xd_dot);

	if (config.mode & TERM_PD)
		control += pd_control;
	if (config.mode & TERM_RISE)
		control += rise_control;
	if (config.mode & TERM_NN)
		control += nn_control;

	info.Wrench = control;
	info.X = x;
	info.X_dot = x_dot;
	info.Xd = xd;
	info.Xd_dot = xd_dot;
	info.V_hat = V_hat_prev;
	info.W_hat = W_hat_prev;
	info.pd_control = pd_control;
	info.rise_control = rise_control;
	info.nn_control = nn_control;
}

Vector6d TrackingController::RiseFeedbackNoAccel(double dt, Vector6d e2) {
	const Gains &g = config.gains;

	Matrix6d ksPlus1 = (Matrix6d)g.ks.asDiagonal() + Matrix6d::Identity();

	Vector6d rise_term_int = ksPlus1*g.alpha.asDiagonal()*e2 + g.beta.asDiagonal()*GetSigns(e2);

	Vector6d rise_term = rise_term_prev + dt / 2.0 * (rise_term_int + rise_term_int_prev);

	Vector6d rise_control = ksPlus1 * e2 + rise_term;

	// Save previous values
	rise_term_prev = rise_term;
	rise_term_int_prev = rise_term_int;

	return rise_control;
}

Vector6d TrackingController::PDFeedback(double dt, Vector6d e2) {
	return config.gains.ks.asDiagonal() * e2;
}

Vector6d TrackingController::NNFeedForward(double dt, Vector6d e2, Vector6d xd, Vector6d xd_dot) {
	const Gains &g = config.gains;

	Vector6d xd_dotdot = (xd_dot - xd_dot_prev) / dt;
	xd_dot_prev = xd_dot;
	Vector6d xd_dotdotdot = (xd_dotdot - xd_dotdot_prev) / dt;
	xd_dotdot_prev = xd_dotdot;

	VectorXd xd_nn(xd.rows()*3+1, 1); // xd_nn = [1 ; xd; xd_dot; xd_dotdot];
	xd_nn << 1, xd, xd_dot, xd_dotdot;

	VectorXd xd_nn_dot(xd.rows()*3+1, 1); // xd_nn_dot = [0 ; xd_dot; xd_dotdot; xd_dotdotdot];
	xd_nn_dot << 0, xd_dot, xd_dotdot, xd_dotdotdot;

	//VectorXd sigma = 2.0 * AttitudeHelpers::Tanh(V_hat.transpose() * xd_nn);
	VectorXd one = VectorXd::Ones(V_hat_prev.cols(), 1); // not sure why I need the matrix form of ::Ones here, the vector form hits a static assert
	cout << one << endl;
	cout << "before sigma" << endl;
	VectorXd sigma = one.cwiseQuotient(one + (-V_hat_prev.transpose() * xd_nn).array().exp().matrix());

	cout << "sigma: " << sigma << endl;

	VectorXd sigma_hat(1+sigma.rows(),1); //sigma_hat = [1; sigma];
	sigma_hat << 1, sigma;

	//VectorXd tempProd = V_hat_prev.transpose() * xd_nn; // 2 * Sech((V_hat.Transpose() * xd_nn).^2);
	//MatrixXd inner = 2.0 * AttitudeHelpers::Sech(tempProd.cwiseProduct(tempProd));
	//MatrixXd sigma_hat_prime_term = inner.asDiagonal();
	MatrixXd sigma_hat_prime_term = sigma.asDiagonal() * (MatrixXd::Identity(sigma.rows(), sigma.rows()) - (MatrixXd)sigma.asDiagonal());
	//sigma_hat_prime = [zeros(1,length(sigma_hat_prime_term));sigma_hat_prime_term];
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

static Matrix6d GetJacobian(const Vector6d& x) {
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

static Matrix6d GetJacobianInverse(const Vector6d& x) {
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

static Vector6d GetSigns(const Vector6d& x) {
	Vector6d signs;

	for (int i = 0; i < 6; i++) {
		if (x(i) > 0)
			signs(i) = 1.0;
		else if (x(i) < 0)
			signs(i) = -1.0;
		else
			signs(i) = 0.0;
	}

	return signs;
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

