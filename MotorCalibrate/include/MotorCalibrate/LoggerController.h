#ifndef MOTORCALIBRATE_LOGGERCONTROLLER_H
#define MOTORCALIBRATE_LOGGERCONTROLLER_H

#include "MotorCalibrate/MotorDriverController.h"
#include "MotorCalibrate/FTSensorLogger.h"
#include <fstream>
#include <QObject>

namespace subjugator {
	class LoggerController : public QObject {
		Q_OBJECT

		public:
			LoggerController(MotorDriverController &motorcontroller, const std::string &device);

		public slots:
			void connect();
			void start(const std::string &filename);
			void stop();

			void tare();

		signals:
			void onNewForce(double force);

		private:
			MotorDriverController &motorcontroller;
			FTSensorLogger sensorlogger;
			std::ofstream logstream;
			bool logging;

			void logCallback(const FTSensorLogger::LogEntry &entry);
	};
}

#endif

