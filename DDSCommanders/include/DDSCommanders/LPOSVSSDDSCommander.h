#ifndef DDSCOMMANDERS_LPOSSDDSCOMMANDER_H
#define DDSCOMMANDERS_LPOSSDDSCOMMANDER_H

#include "DDSCommanders/DepthDDSReceiver.h"
#include "DDSCommanders/DVLDDSReceiver.h"
#include "DDSCommanders/IMUDDSReceiver.h"
#include "DDSCommanders/PDStatusDDSReceiver.h"
#include "SubMain/Workers/SubWorker.h"

namespace subjugator {
	class LPOSVSSDDSCommander {
		public:
			LPOSVSSDDSCommander(Worker &worker, DDSDomainParticipant *participant);

		private:
			void receivedDepth(const DepthMessage &depth);
			void receivedDVL(const DVLMessage &dvl);
			void receivedIMU(const IMUMessage &imu);
			void receivedPDInfo(const PDStatusMessage &status);

			DepthDDSReceiver depthreceiver;
			DVLDDSReceiver dvlreceiver;
			IMUDDSReceiver imureceiver;
			PDStatusDDSReceiver pdreceiver;

			boost::weak_ptr<InputToken> depthtoken;
			boost::weak_ptr<InputToken> dvltoken;
			boost::weak_ptr<InputToken> imutoken;
			boost::weak_ptr<InputToken> pdtoken;
	};
}

#endif

