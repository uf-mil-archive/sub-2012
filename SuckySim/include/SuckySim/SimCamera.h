#ifndef SIMCAMERA_H
#define SIMCAMERA_H

#include "MissionPlanner/SubMissionCamera.h"
#include "MissionPlanner/SubMissionEnums.h"

#include <Eigen/Dense>
#include <vector>
#include <cmath>
#include <boost/math/constants/constants.hpp>

using namespace Eigen;

namespace subjugator
{
	class Plane
	{
	public:
		enum FrustumPlanesCode
		{
			Top,
			Bottom,
			Left,
			Right,
			Near,
			Far,
		};

		FrustumPlanesCode id;
		Vector3d Normal;
		Vector3d Point;
		double D;

		void Set3Points(const Vector3d& v1, const Vector3d& v2, const Vector3d& v3);
		void SetNormalAndPoint(const Vector3d& normal, const Vector3d& point);
		double getDistance(const Vector3d& point);
	};

	class Frustum
	{
	public:
		enum FrustumResultCode
		{
			Outside,
			Intersect,
			Inside,
		};

		std::vector<Plane> planes;
		Vector3d ntl;
		Vector3d ntr;
		Vector3d nbl;
		Vector3d nbr;
		Vector3d ftl;
		Vector3d ftr;
		Vector3d fbl;
		Vector3d fbr;

		double nearD;
		double farD;
		double ratio;
		double angle;
		double tang;

		double nw;
		double nh;
		double fw;
		double fh;

		Frustum();
		void SetCamInternals(double angle, double ratio, double nearD, double farD);
		void SetCamDef(const Vector3d& camPoint, const Vector3d& lookAt, const Vector3d& up);
		FrustumResultCode PointInFrustum(const Vector3d& point);
	};

	class SimCamera
	{
	public:
		SimCamera(const MissionCamera& cam, double fov, double aspectRatio, double nearP, double farP);

		bool PointInView(const Vector3d& point){ return (frustum.PointInFrustum(point) == Frustum::Inside); }
		void setPosition_NED(const Vector3d& val) { position_NED = val; UpdateOthers(); }
		void setRPY(const Vector3d& val) { rpy = val; UpdateOthers(); }

		Frustum getFrustum() {return frustum;}

	private:
		MissionCamera mCam;
		Vector3d position_NED;
		Vector3d rpy;
		double fovDeg;	// Degrees
		double aspectRatio;
		double nearDistance;
		double farDistance;

		Frustum frustum;

		ObjectIDs::ObjectIDCode searchObject;

		void UpdateOthers();
	};
}

#endif /* SIMCAMERA_H */
