#ifndef SENSORLOGGER_IMUDDSRECEIVER_H
#define SENSORLOGGER_IMUDDSRECEIVER_H

#include "DDSMessages/IMUMessage.h"
#include "DDSMessages/IMUMessageSupport.h"
#include <ndds/ndds_cpp.h>
#include <boost/function.hpp>

namespace subjugator {
	class IMUDDSReceiver : DDSDataReaderListener {
		public:
			typedef boost::function<void (const IMUMessage &message)> Callback;

			IMUDDSReceiver(DDSDomainParticipant *participant, const Callback &callback);
			~IMUDDSReceiver();

		private:
			DDSDomainParticipant *participant;
			DDSTopic *topic;
			IMUMessageDataReader *imureader;

			Callback callback;

			virtual void on_data_available(DDSDataReader *unused);
	};
}

#endif

