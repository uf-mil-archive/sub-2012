#ifndef DEPTHTEST_DEPTHBOARDCONTROLLER_H
#define DEPTHTEST_DEPTHBOARDCONTROLLER_H

#include "DepthTest/HeartBeatSender.h"
#include "HAL/SubHAL.h"
#include "HAL/format/DataObjectEndpoint.h"
#include "DataObjects/DepthBoard/DepthBoardInfo.h"
#include <QObject>
#include <boost/scoped_ptr.hpp>

namespace subjugator {
	class DepthBoardController : public QObject {
		Q_OBJECT

		public:
			DepthBoardController(int depthaddr=4);
			inline const DepthBoardInfo &getDepthInfo() { return depthinfo; }

			inline boost::asio::io_service &getIOService() { return hal.getIOService(); } // needed for LoggerController, TODO ugly...

		public slots:
			void setReference(double reference);

		signals:
			void newInfo();

		private:
			SubHAL hal;
			boost::scoped_ptr<DataObjectEndpoint> endpoint;
			DepthBoardInfo depthinfo;

			HeartBeatSender heartbeatsender;

			void endpointReadCallback(std::auto_ptr<DataObject> &object);
			void endpointStateChangeCallback();
	};
}

#endif

