#include "SubMain/Workers/TrackingController/TrackingController.h"

using namespace subjugator;
using namespace Eigen;
using namespace std;

typedef Matrix<double, 6, 1> Vector6d;

TrackingController::TrackingController()
{
	// Default gains. TODO: load these from a file
	ktemp << 20.0,20.0,80.0,15.0,50.0,20.0;
	kstemp << 150.0,150.0,120.0,40.0,40.0,80.0;
	alphatemp << 50.0,50.0,40.0,15.0,20.0,20.0;
	betatemp << 60.0,60.0,30.0,20.0,10.0,30.0;
	gamma1temp << 1.0,1.0,1.0,1.0,1.0,1.0;
	gamma2temp << 1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0;

	pd_on = false;
	rise_on = true;
	nn_on = false;

	rise_term_prev = Vector6d::Zero();
	rise_term = Vector6d::Zero();
	rise_term_int_prev = Vector6d::Zero();
	rise_term_int = Vector6d::Zero();

	x = Vector6d::Zero();
	x_dot = Vector6d::Zero();
	xd = Vector6d::Zero();
	xd_dot = Vector6d::Zero();

	vb = Vector6d::Zero();

	// NN stuff
	xd = Vector6d::Zero();
	xd_dot = Vector6d::Zero();
	xd_dotdot = Vector6d::Zero();
	xd_dotdotdot = Vector6d::Zero();

	V_hat_dot = Matrix19x5d::Zero();
	V_hat_dot_prev = Matrix19x5d::Zero();
	V_hat = Matrix19x5d::Zero();
	V_hat_prev = Matrix19x5d::Zero();
	W_hat_dot = Matrix6d::Zero();
    W_hat_dot_prev = Matrix6d::Zero();
    W_hat = Matrix6d::Zero();
    W_hat_prev = Matrix6d::Zero();

	currentControl = Vector6d::Zero();

	J = Matrix6d::Zero();
	J_inv = Matrix6d::Zero();
}

// We cheat here and copy the current data to common class level variables so multiple controllers
// theoretically could be run in parallel.
void TrackingController::Update(boost::int64_t currentTick, const TrajectoryInfo& traj, const LPOSVSSInfo& lposInfo)
{
    // Update dt
    double dt = (currentTick - previousTime)*SECPERNANOSEC;
    previousTime = currentTick;

    //Protect the INS against the debugger and non monotonic time
    if((dt <= 0) || (dt > .150))
    	return;

    // NED Position
    x.block<3,1>(0,0) = lposInfo.getPosition_NED();
    x.block<3,1>(3,0) = MILQuaternionOps::Quat2Euler(lposInfo.getQuat_NED_B());

    // NED Velocity
    x_dot.block<3,1>(0,0) = lposInfo.getVelocity_NED();
    x_dot.block<3,1>(3,0) = MILQuaternionOps::QuatRotate(lposInfo.getQuat_NED_B(), lposInfo.getAngularRate_BODY());

    // Body Velocity
    vb.block<3,1>(0,0) = MILQuaternionOps::QuatRotate(MILQuaternionOps::QuatInverse(lposInfo.getQuat_NED_B()), lposInfo.getVelocity_NED());
    vb.block<3,1>(3,0) = lposInfo.getAngularRate_BODY();

    // Save the relevant trajectory data
    xd = traj.getTrajectory();
    xd_dot = traj.getTrajectory_dot();
    //xd_dotdot = traj.getTrajectory_dotdot();	// Needed by neural network
    //xd_dotdotdot = traj.getTrajectory_dotdotdot(); // Needed by neural network

    UpdateJacobianInverse(x);
	//UpdateJacobian(x);

	// Set gains (these will be rotated from body to NED frame also inside this function)
	SetGains(ktemp, kstemp, alphatemp, betatemp, lposInfo);

    lock.lock();

	currentControl = Vector6d::Zero();
	if(pd_on)
		currentControl += PDFeedback(dt);
	if(rise_on && !pd_on)	// only allow rise to be on if pd is off
		currentControl += RiseFeedbackNoAccel(dt);
	if(nn_on && rise_on && !pd_on) // Only allow nn to be added if rise is used
		currentControl += NNFeedForward(dt);

	// We think there's supposed to be a possible transformation here, the math supports it but the sub does not like it. May need to look at this further.
	//currentControl = J*currentControl;

    lock.unlock();
}

Vector6d TrackingController::RiseFeedbackNoAccel(double dt)
{
	e = Vector6d::Zero();
	e.block<3,1>(0,0) = xd.block<3,1>(0,0) - x.block<3,1>(0,0);
	e(3) = AttitudeHelpers::DAngleDiff(x(3), xd(3)); // This does b-a ( or xd-x)
	e(4) = AttitudeHelpers::DAngleDiff(x(4), xd(4));
	e(5) = AttitudeHelpers::DAngleDiff(x(5), xd(5));

	Vector6d vbd = J_inv * (k * e + xd_dot);
	e2 = vbd - vb;

	Vector6d sign_term = GetSigns(e);

	rise_term_int = ksPlus1*alpha*e2 + beta*sign_term;

	rise_term = rise_term_prev + dt / 2.0 * (rise_term_int + rise_term_int_prev);

	rise_control = ksPlus1 * e2 + rise_term;

	// Save previous values
	rise_term_prev = rise_term;
	rise_term_int_prev = rise_term_int;

	return rise_control;
}

Vector6d TrackingController::PDFeedback(double dt)
{
//	cout << "GAINS" << endl;
//	cout << "K x: " << k(0) << " y: " << k(1) << " z: " << k(2) << " roll: " << k(3) << " pitch: " << k(4) << " yaw: " << k(5);
//	cout << "Ks x: " << ks(0) << " y: " << ks(1) << " z: " << ks(2) << " roll: " << ks(3) << " pitch: " << ks(4) << " yaw: " << ks(5);
//	cout << "A x: " << alpha(0) << " y: " << alpha(1) << " z: " << alpha(2) << " roll: " << alpha(3) << " pitch: " << alpha(4) << " yaw: " << alpha(5);
//	cout << "B x: " << beta(0) << " y: " << beta(1) << " z: " << beta(2) << " roll: " << beta(3) << " pitch: " << beta(4) << " yaw: " << beta(5);

	e = Vector6d::Zero();
	e.block<3,1>(0,0) = xd.block<3,1>(0,0) - x.block<3,1>(0,0);
	e(3) = AttitudeHelpers::DAngleDiff(x(3), xd(3));
	e(4) = AttitudeHelpers::DAngleDiff(x(4), xd(4));
	e(5) = AttitudeHelpers::DAngleDiff(x(5), xd(5));

	Vector6d vbd = J_inv * (k * e + xd_dot);
	e2 = vbd - vb;

	pd_control = ks * e2;

	return pd_control;
}

Vector6d TrackingController::NNFeedForward(double dt)
{
    VectorXd xd_nn(xd.rows()*3+1,1); 		// xd_nn = [1 ; xd; xd_dot; xd_dotdot];
    xd_nn(0,0) = 1.0;
    xd_nn.block(xd.rows(),1,1,0) = xd;
    xd_nn.block(xd.rows(),1,1+xd.rows(),0) = xd_dot;
    xd_nn.block(xd.rows(),1,1+2*xd.rows(),0) = xd_dotdot;

    VectorXd xd_nn_dot(xd.rows()*3+1,1);	// xd_nn_dot = [0 ; xd_dot; xd_dotdot; xd_dotdotdot];
	xd_nn_dot(0,0) = 0.0;
    xd_nn_dot.block(xd.rows(),1,1,0) = xd_dot;
    xd_nn_dot.block(xd.rows(),1,1+xd.rows(),0) = xd_dotdot;
    xd_nn_dot.block(xd.rows(),1,1+2*xd.rows(),0) = xd_dotdotdot;

    VectorXd sigma = 2.0 * AttitudeHelpers::Tanh(V_hat.transpose() * xd_nn);

    VectorXd sigma_hat(1+sigma.rows(),1);          //sigma_hat = [1; sigma];
    sigma_hat(0,0) = 1.0;
    sigma_hat.block(sigma.rows(),1,1,0) = sigma;

	VectorXd tempProd = V_hat.transpose() * xd_nn;	// 2 * Sech((V_hat.Transpose() * xd_nn).^2);
    MatrixXd inner = 2.0 * AttitudeHelpers::Sech(tempProd.cwiseProduct(tempProd));
    MatrixXd sigma_hat_prime_term = AttitudeHelpers::DiagMatrixFromVector(inner);
    //sigma_hat_prime = [zeros(1,length(sigma_hat_prime_term));sigma_hat_prime_term];
    MatrixXd sigma_hat_prime(1+sigma_hat_prime_term.rows(), sigma_hat_prime_term.cols());
    sigma_hat_prime.fill(0.0);
    sigma_hat_prime.block(sigma_hat_prime_term.rows(), sigma_hat_prime_term.cols(), 1, 0) = sigma_hat_prime_term;

    W_hat_dot = gamma1 * sigma_hat_prime * V_hat.transpose() * xd_nn_dot * e2.transpose();
    V_hat_dot = gamma2 * xd_nn_dot * e2.transpose() * W_hat.transpose() * sigma_hat_prime;

    // integrate W_hat_dot and V_hat_dot
    W_hat = W_hat_prev + dt / 2.0 * (W_hat_dot + W_hat_dot_prev);
    V_hat = V_hat_prev + dt / 2.0 * (V_hat_dot + V_hat_dot_prev);

    nn_control = W_hat.transpose() * sigma_hat;

    // save previous values
    W_hat_prev = W_hat;
    W_hat_dot_prev = W_hat_dot;
    V_hat_prev = V_hat;
    V_hat_dot_prev = V_hat_dot;

	return nn_control;
}

void TrackingController::UpdateJacobian(const Vector6d& x)
{
	double sphi = sin(x(3));
	double cphi = cos(x(3));

	double stheta = sin(x(4));
	double ctheta = cos(x(4));
	double tantheta = tan(x(4));

	double spsi = sin(x(5));
	double cpsi = cos(x(5));

	J.block<3,3>(0,0) <<
            cpsi * ctheta,
            -spsi * cphi + cpsi * stheta * sphi,
            spsi * sphi + cpsi * cphi * stheta,
            spsi * ctheta,
            cpsi * cphi + sphi * stheta * spsi,
            -cpsi * sphi + stheta * spsi * cphi,
            -stheta,
            ctheta * sphi,
            ctheta * cphi;

    J.block<3,3>(3,3) <<
            1,
            sphi * tantheta,
            cphi * tantheta,
            0,
            cphi,
            -sphi,
            0,
            sphi / ctheta,
            cphi / ctheta;
}

void TrackingController::UpdateJacobianInverse(const Vector6d& x)
{
	double sphi = sin(x(3));
	double cphi = cos(x(3));

	double stheta = sin(x(4));
	double ctheta = cos(x(4));

	double spsi = sin(x(5));
	double cpsi = cos(x(5));

	J_inv.block<3,3>(0,0) <<
            cpsi * ctheta,
            spsi * ctheta,
            -stheta,
            -spsi * cphi + cpsi * stheta * sphi,
            cpsi * cphi + sphi * stheta * spsi,
            ctheta * sphi,
            spsi * sphi + cpsi * cphi * stheta,
            -cpsi * sphi + stheta * spsi * cphi,
            ctheta * cphi;

    J_inv.block<3,3>(3,3) <<
            1,
            0,
            -stheta,
            0,
            cphi,
            ctheta * sphi,
            0,
            -sphi,
            ctheta*cphi;
}

Vector6d TrackingController::GetSigns(const Vector6d& x)
{
	Vector6d signs = Vector6d::Ones();

	for(int i = 0; i < 6; i++)
	{
		if(x(i) > 0)
			signs(i) = 1.0;
		else if(x(i) < 0)
			signs(i) = -1.0;
		else
			signs(i) = 0.0;
	}

	return signs;
}

void TrackingController::InitTimer(boost::int64_t currentTickCount)
{
	previousTime = currentTickCount;
}

void TrackingController::GetWrench(TrackingControllerInfo& info)
{
	lock.lock();

	info.Wrench = currentControl;
	info.X = x;
	info.X_dot = x_dot;
	info.Xd = xd;
	info.Xd_dot = xd_dot;
	info.V_hat = V_hat;
	info.W_hat = W_hat;
	info.pd_control = pd_control;
	info.rise_control = rise_control;
	info.nn_control = nn_control;

	lock.unlock();
}

void TrackingController::SetGains(Vector6d kV, Vector6d ksV, Vector6d alphaV, Vector6d betaV, const LPOSVSSInfo& lposInfo)
{
	// Rotate x,y,z gains from Body into NED
	Vector4d lposQuatNEDBody = lposInfo.getQuat_NED_B();
	kV.block<3,1>(0,0) = MILQuaternionOps::QuatRotate(lposQuatNEDBody, kV.block<3,1>(0,0));
	ksV.block<3,1>(0,0) = MILQuaternionOps::QuatRotate(lposQuatNEDBody, ksV.block<3,1>(0,0));
	alphaV.block<3,1>(0,0) = MILQuaternionOps::QuatRotate(lposQuatNEDBody, alphaV.block<3,1>(0,0));
	betaV.block<3,1>(0,0) = MILQuaternionOps::QuatRotate(lposQuatNEDBody, betaV.block<3,1>(0,0));

	k = AttitudeHelpers::DiagMatrixFromVector(kV);
	ks = AttitudeHelpers::DiagMatrixFromVector(ksV);
	alpha = AttitudeHelpers::DiagMatrixFromVector(alphaV);
	beta = AttitudeHelpers::DiagMatrixFromVector(betaV);
	ksPlus1 = ks + Matrix<double,6,6>::Identity();
}
