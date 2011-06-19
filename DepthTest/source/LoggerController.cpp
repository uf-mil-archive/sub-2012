#include "DepthTest/LoggerController.h"
#include "boost/date_time/posix_time/posix_time.hpp"
#include <cmath>

using namespace subjugator;
using namespace boost;
using namespace boost::posix_time;
using namespace std;

LoggerController::LoggerController(DepthBoardController &depthcontroller, const std::string &device)
: depthcontroller(depthcontroller),
  logging(false) {
}

void LoggerController::start(const std::string &filename) {
	logstream.open(filename.c_str());
	logging = true;
	logstream << "Time, Depth" << endl;
}

void LoggerController::stop() {
	logging = false;
	logstream.close();
}

void LoggerController::logCallback() {
	if (logging) {
		const DepthBoardInfo &info = depthcontroller.getDepthInfo();
		logstream << second_clock::local_time().time_of_day() << ", ";
		logstream << info.getDepth() << endl;
	}
}

