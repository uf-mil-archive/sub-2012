/*
 * SubHydrophoneWorker.h
 *
 *  Created on: Jul 1, 2011
 *      Author: gbdash
 */

#ifndef SUBHYDROPHONEWORKER_H_
#define SUBHYDROPHONEWORKER_H_

#include "SubMain/SubPrerequisites.h"
#include "SubMain/Workers/SubWorker.h"
#include "HAL/HAL.h"
#include "HAL/SubHAL.h"
#include "Hydrophone/HydrophoneDataProcessor.h"
#include "DataObjects/Hydrophone/HydrophonePacketFormatter.h"
#include "DataObjects/Hydrophone/HydrophoneDataObjectFormatter.h"

namespace subjugator
{
	class HydrophoneWorkerCommands
	{
	public:
		enum HydrophoneWorkerCommandCode
		{
			logData = 0,
		};
	};

	class HydrophoneWorker : public Worker
	{
		public:
			HydrophoneWorker(boost::asio::io_service& io, int64_t rate, std::string configPath);
			~HydrophoneWorker(){}

			bool Startup();

			HydrophoneDataProcessor::Config config;

		private:
			SubHAL hal;
			std::auto_ptr<DataObjectEndpoint> pEndpoint;

			void readyState();
			void emergencyState();
			void failState();
			void halReceiveCallback(std::auto_ptr<DataObject> &dobj);
			void halStateChangeCallback();
			void logNewData(const DataObject& dobj);
	};
}

#endif /* SUBHYDROPHONEWORKER_H_ */
