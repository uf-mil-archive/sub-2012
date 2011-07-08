#include <QtGui/QApplication>
#include "DDSWatcher/mainwindow.h"

using namespace subjugator;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    DDSDomainParticipant *participant = DDSDomainParticipantFactory::get_instance()->create_participant(0, DDS_PARTICIPANT_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
		if (!participant)
			throw runtime_error("Failed to create DDSDomainParticipant for Trajectory Test");

	if (LPOSVSSMessageTypeSupport::register_type(participant, LPOSVSSMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
			throw runtime_error("Failed to register type");

	if (SetWaypointMessageTypeSupport::register_type(participant, SetWaypointMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
				throw runtime_error("Failed to register type");

	if (DepthMessageTypeSupport::register_type(participant, DepthMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
					throw runtime_error("Failed to register type");

	if (IMUMessageTypeSupport::register_type(participant, IMUMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
					throw runtime_error("Failed to register type");

	if (DVLMessageTypeSupport::register_type(participant, DVLMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
					throw runtime_error("Failed to register type");

	if (PDStatusMessageTypeSupport::register_type(participant, PDStatusMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
						throw runtime_error("Failed to register type");

	if (HydrophoneMessageTypeSupport::register_type(participant, HydrophoneMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
							throw runtime_error("Failed to register type");

	if (FinderMessageListTypeSupport::register_type(participant, FinderMessageListTypeSupport::get_type_name()) != DDS_RETCODE_OK)
							throw runtime_error("Failed to register type");

	if (TrajectoryMessageTypeSupport::register_type(participant, TrajectoryMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
								throw runtime_error("Failed to register type");

    MainWindow w(participant);
    w.show();

    return a.exec();
}
