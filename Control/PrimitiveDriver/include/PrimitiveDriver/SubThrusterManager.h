#ifndef SUBTHRUSTERMANAGER_H
#define SUBTHRUSTERMANAGER_H

#include "SubMain/SubPrerequisites.h"
#include "PrimitiveDriver/SubThrusterMapper.h"
#include "PrimitiveDriver/SubThruster.h"
#include <Eigen/Dense>

namespace subjugator
{
	class ThrusterManager
	{
	public:
		typedef Matrix<double, 6, 1> Vector6D;
	public:
		ThrusterManager(boost::shared_ptr<SubHAL> h);
		//ThrusterManager(boost::shared_ptr<SubHAL> h, std::string fileName); // some day config files (maybe seperate config struct?)

		void addThruster(Thruster *t);
		void RebuildMapper();
		void ImplementScrew(const Vector6D& screw);
		void SetOriginToCOM(Vector3d pCom);
		bool IsReady(){ return mIsReady; }
		double getCurrent(int thruster) { return mThrusters[thruster]->getInfo().getCurrent(); }

	private:
		boost::shared_ptr<SubHAL> mHal;
		std::vector<boost::shared_ptr<Thruster> > mThrusters;
		std::auto_ptr<ThrusterMapper> mThrusterMapper;
		Vector3d mOriginToCOM;
		bool mIsReady;
	};
}

#endif /* SUBTHRUSTERMANAGER_H */
