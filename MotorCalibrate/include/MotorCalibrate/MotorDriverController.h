#ifndef MOTORCALIBRATE_MOTORDRIVERCONTROLLER_H
#define MOTORCALIBRATE_MOTORDRIVERCONTROLLER_H

#include "MotorCalibrate/HeartBeatSender.h"
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

		public slots:
			void setReference(double reference);

		signals:
			void newInfo(const MotorDriverInfo &info);

		private:
			SubHAL hal;
			boost::scoped_ptr<DataObjectEndpoint> endpoint;

			HeartBeatSender heartbeatsender;

			void endpointReadCallback(std::auto_ptr<DataObject> &object);
			void endpointStateChangeCallback();
	};
}

#endif

