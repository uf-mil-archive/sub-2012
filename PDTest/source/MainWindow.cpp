#include "PDTest/MainWindow.h"
#include "DDSMessages/PDWrenchMessage.h"
#include "DDSMessages/PDActuatorMessage.h"
#include <boost/bind.hpp>
#include <iostream>

using namespace subjugator;
using namespace boost;
using namespace std;

MainWindow::MainWindow()
: deactivatetimer(this),
  sender(dds.participant, "PDWrench"),
  actuatorsender(dds.participant, "PDActuator"),
  receiver(dds.participant, "PDStatus", bind(&MainWindow::onPDStatusMessage, this, _1)) {
	ui.setupUi(this);
	deactivatetimer.setSingleShot(true);

	connect(ui.sendButton, SIGNAL(clicked()), this, SLOT(onSendButtonClicked()));
	connect(ui.stopButton, SIGNAL(clicked()), this, SLOT(onStopButtonClicked()));
	connect(&deactivatetimer, SIGNAL(timeout()), this, SLOT(onDeactivateTimer()));
	connect(this, SIGNAL(pdStatusUpdated()), this, SLOT(onPDStatusUpdated()));
}

void MainWindow::onPDStatusMessage(const PDStatusMessage &pdstatus) {
	this->pdstatus = pdstatus;
	emit pdStatusUpdated();
}

void MainWindow::onPDStatusUpdated() {
	ui.grabberLeftStatusLabel->setText(pdstatus.flags & PDSTATUSFLAG_GRABBERLEFT ? "Open" : "Closed");
	ui.grabberRightStatusLabel->setText(pdstatus.flags & PDSTATUSFLAG_GRABBERRIGHT ? "Open" : "Closed");
	ui.ballDropperStatusLabel->setText(pdstatus.flags & PDSTATUSFLAG_BALLDROPPER ? "On" : "Off");
	ui.shooterLeftStatusLabel->setText(pdstatus.flags & PDSTATUSFLAG_SHOOTERLEFT ? "On" : "Off");
	ui.shooterRightStatusLabel->setText(pdstatus.flags & PDSTATUSFLAG_SHOOTERRIGHT ? "On" : "Off");
	ui.limitSwitchStatusLabel->setText(pdstatus.flags & PDSTATUSFLAG_LIMITSWITCH ? "Closed" : "Open");
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

	PDActuatorMessage actmsg;
	actmsg.flags = genFlagsFromCheckboxes();
	actuatorsender.Send(actmsg);

	deactivatetimer.start(500);
}

void MainWindow::onDeactivateTimer() {
	PDActuatorMessage actmsg;
	actmsg.flags = genFlagsFromCheckboxes();
	actmsg.flags &= ~(PDACTUATORFLAG_SHOOTERLEFT | PDACTUATORFLAG_SHOOTERRIGHT | PDACTUATORFLAG_BALLDROPPER);
	actuatorsender.Send(actmsg);
}

void MainWindow::onStopButtonClicked() {
	PDWrenchMessage msg;
	for (int i=0; i<3; i++) {
		msg.linear[i] = 0;
		msg.moment[i] = 0;
	}
	sender.Send(msg);
}

int MainWindow::genFlagsFromCheckboxes() const {
	int flags=0;

	if (ui.grabberLeftCheckBox->isChecked())
		flags |= PDACTUATORFLAG_GRABBERLEFT;
	if (ui.grabberRightCheckBox->isChecked())
		flags |= PDACTUATORFLAG_GRABBERRIGHT;
	if (ui.ballDropperCheckBox->isChecked())
		flags |= PDACTUATORFLAG_BALLDROPPER;
	if (ui.shooterLeftCheckBox->isChecked())
		flags |= PDACTUATORFLAG_SHOOTERLEFT;
	if (ui.shooterRightCheckBox->isChecked())
		flags |= PDACTUATORFLAG_SHOOTERRIGHT;

	return flags;
}

MainWindow::DDSHelper::DDSHelper() {
	participant = DDSDomainParticipantFactory::get_instance()->create_participant(0, DDS_PARTICIPANT_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
	if (!participant)
		throw runtime_error("Failed to create DDSDomainParticipant");

	if (PDWrenchMessageTypeSupport::register_type(participant, PDWrenchMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
		throw runtime_error("Failed to register type");

	if (PDActuatorMessageTypeSupport::register_type(participant, PDActuatorMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
		throw runtime_error("Failed to register type");

	if (PDStatusMessageTypeSupport::register_type(participant, PDStatusMessageTypeSupport::get_type_name()) != DDS_RETCODE_OK)
		throw runtime_error("Failed to register type");
}

MainWindow::DDSHelper::~DDSHelper() {
	//delete participant; // TODO, private destructor?
}

