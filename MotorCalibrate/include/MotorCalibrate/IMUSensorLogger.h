#ifndef MOTORCALIBRATE_IMUSENSOR_H
#define MOTORCALIBRATE_IMUSENSOR_H

#include "DDSCommanders/IMUDDSReceiver.h"
#include "MotorCalibrate/MotorDriverController.h"
#include <QObject>
#include <fstream>

namespace subjugator {
	class IMUSensorLogger : public QObject {
		Q_OBJECT

		public:
			IMUSensorLogger(MotorDriverController &motorcontroller);

			const IMUMessage &getMessage() const { return msg; }

		public slots:
			void start(const std::string &filename);
			void stop();

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

			MotorDriverController &motorcontroller;
			DDSHelper dds;
			IMUDDSReceiver imureceiver;
			IMUMessage msg;
			std::ofstream logstream;
			bool logging;
	};
}

#endif

