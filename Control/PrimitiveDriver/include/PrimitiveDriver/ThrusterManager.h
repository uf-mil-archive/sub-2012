#ifndef SUBTHRUSTERMANAGER_H
#define SUBTHRUSTERMANAGER_H

#include "PrimitiveDriver/ThrusterMapper.h"
#include "PrimitiveDriver/Thruster.h"
#include "LibSub/Worker/WorkerStateUpdater.h"

#include <Eigen/Dense>

namespace subjugator {
	class ThrusterManager : public WorkerStateUpdaterContainer {
		public:
			typedef Matrix<double, 6, 1> Vector6D;

			ThrusterManager(HAL &h);

			void addThruster(Thruster *t);
			void RebuildMapper();
			void ImplementScrew(const Vector6D& screw);
			void SetOriginToCOM(Vector3d pCom);
			bool IsReady(){ return mIsReady; }
			double getCurrent(int thruster) { return mThrusters[thruster]->getInfo().getCurrent(); }

		private:
			HAL &hal;
			std::vector<boost::shared_ptr<Thruster> > mThrusters;
			std::auto_ptr<ThrusterMapper> mThrusterMapper;
			Vector3d mOriginToCOM;
			bool mIsReady;
	};
}

#endif /* SUBTHRUSTERMANAGER_H */
