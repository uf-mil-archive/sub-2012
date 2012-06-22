#ifndef PRIMITIVEDRIVER_ACTUATORMANAGER_H
#define PRIMITIVEDRIVER_ACTUATORMANAGER_H

#include "HAL/HAL.h"
#include "LibSub/State/StateUpdater.h"
#include "LibSub/Worker/WorkerEndpoint.h"
#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>

namespace subjugator {
	class ActuatorManager : public StateUpdaterContainer {
	public:
		static const int ACTUATOR_COUNT=6;
		static const int SWITCH_COUNT=2;

		ActuatorManager(HAL &hal, const std::string &actuatorendpointconf);

		virtual void updateState(double dt);

		bool getSwitch(int num) const { return switches[num]; }

		void setActuator(int num, bool state);
		void setActuators(const std::vector<bool> &actuators);
		bool getActuator(int num) const { return actuators[num]; }
		const std::vector<bool> &getActuators() const { return actuators; }

		void offActuators();

	private:
		WorkerEndpoint endpoint;

		std::vector<bool> actuators;
		std::vector<bool> switches;

		double switchreadtimer;

		void actuatorReceiveCallback(const boost::shared_ptr<DataObject> &data);
		void sendSetValves();
		void sendReadSwitches();
	};
};

#endif
