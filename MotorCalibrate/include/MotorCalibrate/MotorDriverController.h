#ifndef MOTORCALIBRATE_MOTORDRIVERCONTROLLER_H
#define MOTORCALIBRATE_MOTORDRIVERCONTROLLER_H

#include "MotorCalibrate/HeartBeatSender.h"
#include "MotorCalibrate/MotorRamper.h"
#include "HAL/SubHAL.h"
#include "HAL/format/DataObjectEndpoint.h"
#include "DataObjects/MotorDriver/MotorDriverInfo.h"
#include <QObject>
#include <boost/scoped_ptr.hpp>

namespace subjugator {
	class MotorDriverController : public QObject {
		Q_OBJECT

		public:
			MotorDriverController(int motaddr=2);
			inline const MotorDriverInfo &getMotorInfo() { return motorinfo; }

		public slots:
			void setReference(double reference);
			void startRamp(const MotorRamper::Settings &settings);
			void stopRamp();

		signals:
			void newInfo();
			void newRampReference(double reference);

		private:
			SubHAL hal;
			boost::scoped_ptr<DataObjectEndpoint> endpoint;
			MotorDriverInfo motorinfo;

			HeartBeatSender heartbeatsender;
			MotorRamper motorramper;

			void endpointReadCallback(std::auto_ptr<DataObject> &object);
			void endpointStateChangeCallback();

			void rampUpdateCallback(double reference);
			void rampCompleteCallback();
	};
}

#endif

