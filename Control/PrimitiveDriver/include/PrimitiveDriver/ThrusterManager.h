#ifndef SUBTHRUSTERMANAGER_H
#define SUBTHRUSTERMANAGER_H

#include "PrimitiveDriver/Thruster.h"
#include "LibSub/State/StateUpdater.h"
#include <boost/ptr_container/ptr_vector.hpp>
#include <Eigen/Dense>

namespace subjugator {
	class ThrusterManager : public StateUpdater {
		public:
			typedef boost::function<void (int num, const State &state)> ThrusterChangeCallback;

			ThrusterManager(HAL &hal, int srcaddress, const ThrusterChangeCallback &callback);

			int addThruster(int address); // returns the thrusters number
			Thruster &getThruster(int num) { return thrusters[num]; }
			const Thruster &getThruster(int num) const { return thrusters[num]; }

			void setEffort(int num, double effort) { thrusters[num].setEffort(effort); }
			boost::shared_ptr<MotorDriverInfo> getInfo(int num) const { return thrusters[num].getInfo(); }
			const State getState(int num) const { return thrusters[num].getState(); }

			void setEfforts(const Eigen::VectorXd &efforts);
			void zeroEfforts();
			int getOnlineThrusterCount() const;

			virtual void updateState(double dt);
			virtual const State &getState() const { return state; }

		private:
			HAL &hal;
			int srcaddress;
			ThrusterChangeCallback callback;

			State state;
			typedef boost::ptr_vector<Thruster> ThrusterVec;
			ThrusterVec thrusters;
	};
}

#endif /* SUBTHRUSTERMANAGER_H */
