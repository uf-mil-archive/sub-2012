#include "LPOSVSS/SubINS.h"
#include <iostream>

using namespace subjugator;
using namespace std;
using namespace Eigen;

/**
 * The constructor for the INS
 *
 * @param[in]	lat				The local latitude(rad)
 * @param[in]	w_dif_prev		The previous unbiased gyro reading minus earth rotation if used(rad/s)
 * @param[in]	a_body_prev		The previous unbiased accelerometer reading(m/s^2)
 * @param[in]	p_prev			The initial NED position(m)
 * @param[in]	v_prev			The initial NED velocity(m)
 * @param[in]	g				The local gravity vector expressed in NED(m/s^2)
 * @param[in]	q_prev			The initial quaternion
 * @param[in]	w_bias			rad/s
 * @param[in]	a_bias			m/s^2
 * @param[in]	q_SUB_IMU
 * @param[in] 	imuTime
 *
 */
INS::INS(double lat, Vector3d w_dif_prev, Vector3d a_body_prev, Vector3d p_prev,
			Vector3d v_prev, Vector3d g, Vector4d q_prev, Vector3d w_bias, Vector3d a_bias,
			Vector4d q_SUB_IMU, boost::uint64_t imuTime):
			lat(lat), w_dif_prev(w_dif_prev), a_body_prev(a_body_prev), p_prev(p_prev),
			v_prev(v_prev), g(g), gMag(std::abs(g(2))), q_prev(q_prev), w_bias(w_bias), a_bias(a_bias),
			q_SUB_IMU(q_SUB_IMU), prevData(new INSData(p_prev, v_prev, q_prev, g, a_body_prev, a_body_prev, w_dif_prev, a_bias, w_bias)),
			imuPreviousTime(imuTime)
{
	w_ie_n(0) = w_ie_e*std::cos(lat);
	w_ie_n(1) = 0.0;
	w_ie_n(2) = -1.0*w_ie_e*std::sin(lat);

	initialized = true;
}

void INS::Update(const IMUInfo& info)
{
	// Validate the contents of the packet
    Vector3d w_body = MILQuaternionOps::QuatRotate(q_SUB_IMU, info.getAngularRate());
    Vector3d a_body = -1.0*gMag*MILQuaternionOps::QuatRotate(q_SUB_IMU, info.getAcceleration());	// Convert the IMU to m/s^2 - ADIS wtf coordinate system

    // Update dt
    dt = (info.getTimestamp() - imuPreviousTime)*SECPERNANOSEC;
    imuPreviousTime = info.getTimestamp();

    //Protect the INS against the debugger and non monotonic time
    if((dt <= 0) || (dt > .050))
    {
    	return;
    }

    if(a_body.norm() > MAX_ACC_MAG)
    {
    	return;
    }
    if(w_body.norm() > MAX_ANG_RATE)
    	return;

	// Lock so a reset can't overwrite mid way through
	lock.lock();

    w_en_n(0) = v_prev(1) / (r_earth - p_prev(2));
    w_en_n(1) = -v_prev(0) / (r_earth - p_prev(2));
    w_en_n(2) = -v_prev(1) * tan(lat) / (r_earth - p_prev(2));

    // Rotate w_ie_n from NED to body - commented out since our IMU can't measure earth's rotation. This just adds noise
    Vector3d w_ie_b(0.0,0.0,0.0);// = MILQuaternionOps::QuatRotate(MILQuaternionOps::QuatInverse(q_prev), w_ie_n);
    Vector3d w_dif = (w_body - w_bias);// - w_ie_b;
    Vector3d sigma = dt / 2.0 *(w_dif + w_dif_prev);

    double hsigma_squared = 0.25 * sigma.dot(sigma);
    double a_c = 1.0 - hsigma_squared / 2.0 + hsigma_squared*hsigma_squared / 24.0;
    double a_s = 0.5 * (1.0 - hsigma_squared / 6.0 + hsigma_squared*hsigma_squared / 120.0 );

    Vector4d r_k(a_c, a_s*sigma(0), a_s*sigma(1), a_s*sigma(2));
    q = MILQuaternionOps::QuatNormalize(MILQuaternionOps::QuatMultiply(q_prev, r_k));

    // This handles sign problems that happen with the attitude helpers - something to do with Subrat its a little fuzzy now
    if(q[0] < 0)
    	q *= -1;

    // Integrate body forces
    Vector3d a_dif = (a_body - a_bias);
    Vector3d v_int = dt / 2.0 * (a_dif + a_body_prev);

    Vector3d u_n = MILQuaternionOps::QuatRotate(q_prev, (v_int + ((.5*sigma).cross(v_int))));

    // Sum all the components to get velocity (NED)
    v = v_prev + u_n + dt * g -	(2.0 * dt * AttitudeHelpers::VectorSkew3(w_ie_n) - dt * AttitudeHelpers::VectorSkew3(w_en_n)) * v_prev;

    // Integrate velocity to get position
    p = p_prev + dt / 2.0 * (v + v_prev);

    // Save previous values
    w_dif_prev = w_dif;
    a_body_prev = a_dif;
    p_prev = p;
    v_prev = v;
    q_prev = q;

    Vector3d g_body = MILQuaternionOps::QuatRotate(MILQuaternionOps::QuatInverse(q), g);
    //Vector3d a_body_no_gravity = a_dif + g_body;

    datalock.lock();
    prevData = boost::shared_ptr<INSData>(new INSData(p, v, q, g_body, a_dif, a_body, w_dif, a_bias, w_bias));
    datalock.unlock();

    lock.unlock();
}

void INS::Reset(const KalmanData& kData, bool tare, const Vector3d& tarePosition)
{
	lock.lock();

	if(tare)
		p_prev = tarePosition;
	else
		p_prev -= kData.PositionErrorEst;

	v_prev -= kData.VelocityError;

	q_prev = MILQuaternionOps::QuatMultiply(q_prev, kData.ErrorQuaternion);
	//a_bias = kData.Acceleration_bias;
	//w_bias = kData.Gyro_bias;

	Vector3d g_body = MILQuaternionOps::QuatRotate(MILQuaternionOps::QuatInverse(q_prev), g);
	Vector3d a_body_no_gravity = a_body_prev - a_bias;
	Vector3d w_dif_temp = w_dif_prev - w_bias;

	datalock.lock();
	prevData = boost::shared_ptr<INSData>(new INSData(p_prev, v_prev, q_prev, g_body, a_body_no_gravity, a_body_no_gravity, w_dif_temp, a_bias, w_bias));
	datalock.unlock();

	lock.unlock();
}

