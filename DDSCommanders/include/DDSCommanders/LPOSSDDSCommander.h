#ifndef DDSCOMMANDERS_LPOSSDDSCOMMANDER_H
#define DDSCOMMANDERS_LPOSSDDSCOMMANDER_H

#include "DDSCommanders/DepthDDSReceiver.h"
#include "DDSCommanders/DVLDDSReceiver.h"
#include "DDSCommanders/IMUDDSReceiver.h"

namespace subjugator {
	class LPOSSDDSCommander {
		public:
			LPOSDDSCommander();

		private:
			void receivedDepth(const DepthMessage &depth);
			void receivedDVL(const DVLMessage &dvl);
			void receivedIMU(const IMUMessage &imu);

			DepthDDSReceiver depthreceiver;
			DVLDDSReceiver dvlreceiver;
			IMUDDSReceiver imureceiver;
	};
}

#endif

