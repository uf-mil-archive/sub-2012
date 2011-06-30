#include "PDTest/MainWindow.h"
#include "DDSMessages/PDWrenchMessage.h"
#include <iostream>

using namespace subjugator;
using namespace std;

MainWindow::MainWindow() : sender(dds.participant, "PDWrench") {
	ui.setupUi(this);

	connect(ui.sendButton, SIGNAL(clicked()), this, SLOT(onSendButtonClicked()));
	connect(ui.stopButton, SIGNAL(clicked()), this, SLOT(onStopButtonClicked()));
}

void MainWindow::onSendButtonClicked() {
	PDWrenchMessage msg;
	msg.linear[0] = ui.linearXSpinBox->value();
	msg.linear[1] = ui.linearYSpinBox->value();
	msg.linear[2] = ui.linearZSpinBox->value();
	msg.moment[0] = ui.momentXSpinBox->value();
	msg.moment[1] = ui.momentYSpinBox->value();
	msg.moment[2] = ui.momentZSpinBox->value();
	sender.Send(msg);
}

void MainWindow::onStopButtonClicked() {
	PDWrenchMessage msg;
	for (int i=0; i<3; i++) {
		msg.linear[i] = 0;
		msg.moment[i] = 0;
	}
	sender.Send(msg);
}

MainWindow::DDSHelper::DDSHelper() {
	participant = DDSDomainParticipantFactory::get_instance()->create_participant(0, DDS_PARTICIPANT_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
	if (!participant)
		throw runtime_error("Failed to create DDSDomainParticipant");

	if (PDWrenchMessageTypeSupport::register_type(participant, PDWrenchMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
		throw runtime_error("Failed to register type");
}

MainWindow::DDSHelper::~DDSHelper() {
	//delete participant; // TODO, private destructor?
}

