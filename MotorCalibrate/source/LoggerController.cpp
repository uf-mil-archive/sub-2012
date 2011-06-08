#include "MotorCalibrate/LoggerController.h"
#include "boost/date_time/posix_time/posix_time.hpp"
#include <cmath>

using namespace subjugator;
using namespace boost;
using namespace boost::posix_time;
using namespace std;

LoggerController::LoggerController(MotorDriverController &motorcontroller, const std::string &device)
: motorcontroller(motorcontroller),
  sensorlogger(device, motorcontroller.getIOService(), boost::bind(&LoggerController::logCallback, this, _1)),
  logging(false) {
}

void LoggerController::connect() {
	sensorlogger.begin();
}

void LoggerController::start(const std::string &filename) {
	logstream.open(filename.c_str());
	logging = true;
	logstream << "Time, fx, fy, fz, mx, my, mz, RefInput, PresOutput, Vrail, Current" << endl;
}

void LoggerController::stop() {
	logging = false;
	logstream.close();
}

void LoggerController::tare() {
	sensorlogger.end();
	sensorlogger.bias();
	sensorlogger.begin();
}

void LoggerController::logCallback(const FTSensorLogger::LogEntry &entry) {
	double force = sqrt(entry.fx*entry.fx + entry.fy*entry.fy + entry.fz*entry.fz);
	emit onNewForce(force);

	if (logging) {
		const MotorDriverInfo &info = motorcontroller.getMotorInfo();
		logstream << second_clock::local_time().time_of_day() << ", " << entry.fx << ", " << entry.fy << ", " << entry.fz << ", " << entry.mx << ", " << entry.my << ", " << entry.mz << ", ";
		logstream << info.getReferenceInput() << ", " << info.getPresentOutput() << ", " << info.getRailVoltage() << ", " << info.getCurrent() << endl;
	}
}

