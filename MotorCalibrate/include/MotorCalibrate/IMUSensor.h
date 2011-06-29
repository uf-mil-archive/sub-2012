#ifndef MOTORCALIBRATE_IMUSENSOR_H
#define MOTORCALIBRATE_IMUSENSOR_H

#include "DDSCommanders/IMUDDSReceiver.h"
#include <QObject>

namespace subjugator {
	class IMUSensor : public QObject {
		Q_OBJECT

		public:
			IMUSensor();

			const IMUMessage &getMessage() const { return msg; }

		signals:
			void onNewMessage();

		private:
			void ddsCallback(const IMUMessage &imu);

			class DDSHelper {
				public:
					DDSHelper();
					~DDSHelper();

					DDSDomainParticipant *participant;
			};

			DDSHelper dds;
			IMUDDSReceiver imureceiver;
			IMUMessage msg;
	};
}

#endif

