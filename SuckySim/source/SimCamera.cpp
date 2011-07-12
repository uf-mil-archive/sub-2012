#include "SuckySim/SimCamera.h"

using namespace subjugator;
using namespace Eigen;
using namespace std;

void Plane::Set3Points(const Vector3d& v1, const Vector3d& v2, const Vector3d& v3)
{
	Vector3d aux1, aux2;
	aux1 = v1 - v3;
	aux2 = v3 - v2;

	Normal = aux2.cross(aux1);
	Normal.normalize();

	Point = v2;
	D = -1.0*Normal.dot(Point);
}

void Plane::SetNormalAndPoint(const Vector3d& normal, const Vector3d& point)
{
	Normal = normal;
	Normal.normalize();

	Point = point;
	D = -1.0 * Normal.dot(point);
}

double Plane::getDistance(const Vector3d& point)
{
	return (D + Normal.dot(point));
}

Frustum::Frustum()
{
	planes.resize(6);
	planes.clear();

	Plane p;
	for(int i = 0; i < 6; i++)
	{
		p.id = (Plane::FrustumPlanesCode)i;
		planes.push_back(p);	// copies into vector;
	}
}

void Frustum::SetCamInternals(double angle, double ratio, double nearD, double farD)
{
	this->angle = angle;
	this->ratio = ratio;
	this->nearD = nearD;
	this->farD = farD;

	tang = tan(angle * boost::math::constants::pi<double>() / 180.0 * 0.5);
	nh = nearD*tang;
	nw = nh*ratio;
	fh = farD*tang;
	fw = fh*ratio;
}

void Frustum::SetCamDef(const Vector3d& camPoint, const Vector3d& lookAt, const Vector3d& up)
{
	Vector3d nc, fc, X, Y, Z;

	Z = camPoint - lookAt;
	Z.normalize();

	X = up.cross(Z);
	X.normalize();

	Y = Z.cross(X);
	Y.normalize();

	nc = camPoint - nearD*Z;
	fc = camPoint - farD*Z;

	Vector3d Ynh = nh*Y;
	Vector3d Xnw = nw*X;

	ntl = nc + Ynh - Xnw;
	ntr = nc + Ynh + Xnw;
	nbl = nc - Ynh - Xnw;
	nbr = nc - Ynh + Xnw;


	Vector3d Yfh = fh * Y;
	Vector3d Xfw = fw * X;
	ftl = fc + Yfh - Xfw;
	ftr = fc + Yfh + Xfw;
	fbl = fc - Yfh - Xfw;
	fbr = fc - Yfh + Xfw;

    planes[(int)Plane::Top].Set3Points(ntr, ntl, ftl);
    planes[(int)Plane::Bottom].Set3Points(nbl, nbr, fbr);
    planes[(int)Plane::Left].Set3Points(ntl, nbl, fbl);
    planes[(int)Plane::Right].Set3Points(nbr, ntr, fbr);
    planes[(int)Plane::Near].Set3Points(ntl, ntr, nbr);
    planes[(int)Plane::Far].Set3Points(ftr, ftl, fbl);
}

Frustum::FrustumResultCode Frustum::PointInFrustum(const Vector3d& point)
{
	Frustum::FrustumResultCode result = Frustum::Inside;
	for(int i = 0; i < 6; i++)
	{
		if(planes[i].getDistance(point) < 0)
			return Frustum::Outside;
	}

	return result;
}

SimCamera::SimCamera(const MissionCamera& cam, double fov, double aspect, double nearP, double farP) :
		mCam(cam), fovDeg(fov), aspectRatio(aspect), nearDistance(nearP), farDistance(farP)
{
	frustum.SetCamInternals(fovDeg, aspectRatio, nearDistance, farDistance);
}

void SimCamera::UpdateOthers()
{
	Vector3d up, lookAt;
	if(mCam.ID == MissionCameraIDs::Front)
	{
		// The up vector is always -Z
		up = Vector3d(0,0,-1.0);
		// Get a vector that describes where the camera is looking, ie in front of it
		lookAt = Vector3d(cos(rpy(2)), sin(rpy(2)), 0.0) + position_NED;
	}
	else
	{
		lookAt = Vector3d(0,0,1.0) + position_NED;
		up = Vector3d(cos(rpy(2)), sin(rpy(2)), 0.0);
	}

	frustum.SetCamDef(position_NED, lookAt, up);
}

