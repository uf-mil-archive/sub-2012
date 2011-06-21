#ifndef DEPTHTEST_LOGGERCONTROLLER_H
#define DEPTHTEST_LOGGERCONTROLLER_H

#include "DepthTest/DepthController.h"
#include <fstream>
#include <QObject>

namespace subjugator {
	class LoggerController : public QObject {
		Q_OBJECT

		public:
			LoggerController(DepthController &depthcontroller, const std::string &device);

		public slots:
			void start(const std::string &filename);
			void stop();

		private:
			DepthController &depthcontroller;
			std::ofstream logstream;
			bool logging;

			void logCallback();
	};
}

#endif

