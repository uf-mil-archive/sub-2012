#include "SensorLogger/IMUDDSReceiver.h"
#include "DDSMessages/IMUMessage.h"
#include "DDSMessages/IMUMessageSupport.h"
#include <ndds/ndds_cpp.h>

using namespace subjugator;
using namespace boost;
using namespace std;

IMUDDSReceiver::IMUDDSReceiver(DDSDomainParticipant *participant, const Callback &callback) : participant(participant) {
	topic = participant->create_topic("IMU", IMUMessageTypeSupport::get_type_name(), DDS_TOPIC_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
	if (!topic)
		throw runtime_error("Failed to create Topic");

	DDSDataReader *reader = participant->create_datareader(topic, DDS_DATAREADER_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
	if (!reader)
		throw runtime_error("Failed to create DataReader");

	imureader = IMUMessageDataReader::narrow(reader);
	if (!imureader)
		throw runtime_error("Failed to narrow to IMUMessageDataReader");

	if (imureader->set_listener(this, DDS_DATA_AVAILABLE_STATUS) != DDS_RETCODE_OK)
		throw runtime_error("Failed to set listener on the IMUMessageDataReader");
}

IMUDDSReceiver::~IMUDDSReceiver() {
	participant->delete_datareader(imureader);
	participant->delete_topic(topic);
}

void IMUDDSReceiver::on_data_available(DDSDataReader *unused) {
	IMUMessageSeq imuseq;
	DDS_SampleInfoSeq infoseq;
	if (imureader->take(imuseq, infoseq, DDS_LENGTH_UNLIMITED, DDS_ANY_SAMPLE_STATE, DDS_ANY_VIEW_STATE, DDS_ANY_INSTANCE_STATE) != DDS_RETCODE_OK)
		throw runtime_error("Failed to take from IMUMessageDataReader");

	try {
		for (int i=0; i<imuseq.length(); ++i)
			callback(imuseq[i]);

		imuseq.unloan();
		infoseq.unloan();
	} catch (...) {
		imuseq.unloan(); // RTI can't use destructors properly grumble grumble. This is the only way to ensure this won't leak memory in the event of an exception
		infoseq.unloan();
		throw;
	}
}

