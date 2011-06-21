#ifndef DEPTHTEST_DEPTHCONTROLLER_H
#define DEPTHTEST_DEPTHCONTROLLER_H

#include "DepthTest/HeartBeatSender.h"
#include "HAL/SubHAL.h"
#include "HAL/format/DataObjectEndpoint.h"
#include "DataObjects/Depth/DepthInfo.h"
#include <QObject>
#include <boost/scoped_ptr.hpp>

namespace subjugator {
	class DepthController : public QObject {
		Q_OBJECT

		public:
			DepthController(int depthaddr=4);
			inline const DepthInfo &getDepthInfo() { return depthinfo; }

			inline boost::asio::io_service &getIOService() { return hal.getIOService(); } // needed for LoggerController, TODO ugly...

		signals:
			void newInfo();

		private:
			SubHAL hal;
			boost::scoped_ptr<DataObjectEndpoint> endpoint;
			DepthInfo depthinfo;

			HeartBeatSender heartbeatsender;

			void endpointReadCallback(std::auto_ptr<DataObject> &object);
			void endpointStateChangeCallback();
	};
}

#endif

