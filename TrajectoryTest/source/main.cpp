#include <QtGui/QApplication>
#include "TrajectoryTest/mainwindow.h"
#include <stdexcept>

using namespace std;
using namespace subjugator;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    DDSDomainParticipant *participant = DDSDomainParticipantFactory::get_instance()->create_participant(0, DDS_PARTICIPANT_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
    	if (!participant)
    		throw runtime_error("Failed to create DDSDomainParticipant for Trajectory Test");

	if (TrajectoryMessageTypeSupport::register_type(participant, TrajectoryMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
			throw runtime_error("Failed to register type");

	DDSDomainParticipant *partSender = DDSDomainParticipantFactory::get_instance()->create_participant(0, DDS_PARTICIPANT_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
		if (!partSender)
			throw runtime_error("Failed to create DDSDomainParticipant for Trajectory Test");

	if (LocalWaypointDriverMessageTypeSupport::register_type(partSender, LocalWaypointDriverMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
			throw runtime_error("Failed to register type");

    MainWindow w(participant, partSender);
    w.show();

    return a.exec();
}
