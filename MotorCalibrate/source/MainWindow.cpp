#include "MotorCalibrate/MainWindow.h"
#include "DataObjects/MotorDriver/MotorDriverDataObjectFormatter.h"
#include "DataObjects/MotorDriver/SetReference.h"
#include "HAL/format/Sub7EPacketFormatter.h"
#include <iostream>
#include <QFileDialog>
#include <sstream>

using namespace subjugator;
using namespace std;

MainWindow::MainWindow(int haladdr)
: motorcontroller(haladdr),
  logger(motorcontroller, "/dev/ttyUSB0"),
  imulogger(motorcontroller) {
	ui.setupUi(this);
	ui.statusBar->showMessage("No connection to motor driver");

	connect(ui.setReferenceButton, SIGNAL(clicked()), this, SLOT(onSetReferenceButtonClicked()));
	connect(ui.stopReferenceButton, SIGNAL(clicked()), this, SLOT(onStopReferenceButtonClicked()));
	connect(ui.startRampButton, SIGNAL(clicked()), this, SLOT(onStartRampButtonClicked()));
	connect(ui.stopRampButton, SIGNAL(clicked()), this, SLOT(onStopRampButtonClicked()));
	connect(ui.startBangButton, SIGNAL(clicked()), this, SLOT(onStartBangButtonClicked()));
	connect(ui.stopBangButton, SIGNAL(clicked()), this, SLOT(onStopBangButtonClicked()));
	connect(ui.connectButton, SIGNAL(clicked()), this, SLOT(onConnectButtonClicked()));
	connect(ui.tareButton, SIGNAL(clicked()), &logger, SLOT(tare()));
	connect(ui.startLogButton, SIGNAL(clicked()), this, SLOT(onStartLogButtonClicked()));
	connect(ui.stopLogButton, SIGNAL(clicked()), this, SLOT(onStopLogButtonClicked()));
	connect(ui.browseButton, SIGNAL(clicked()), this, SLOT(onBrowseButtonClicked()));
	connect(&motorcontroller, SIGNAL(newInfo()), this, SLOT(onNewMotorInfo()));
	connect(&motorcontroller, SIGNAL(newRampReference(double)), this, SLOT(onNewRampReference(double)));
	connect(&motorcontroller, SIGNAL(newBangReference(double)), this, SLOT(onNewBangReference(double)));
	connect(&motorcontroller, SIGNAL(rampComplete()), this, SLOT(onRampComplete()));
	connect(&logger, SIGNAL(onNewForce(double)), this, SLOT(onNewForce(double)));
	connect(ui.startMagLogButton, SIGNAL(clicked()), this, SLOT(onStartMagLogButtonClicked()));
	connect(ui.stopMagLogButton, SIGNAL(clicked()), this, SLOT(onStopMagLogButtonClicked()));
	connect(ui.magBrowseButton, SIGNAL(clicked()), this, SLOT(onMagBrowseButtonClicked()));
	connect(&imulogger, SIGNAL(onNewMessage()), this, SLOT(onNewIMU()));

	stringstream buf;
	buf << "Motor calibrate on address " << haladdr;
	setWindowTitle(QString(buf.str().c_str()));
}

void MainWindow::onNewMotorInfo() {
	const MotorDriverInfo &info = motorcontroller.getMotorInfo();
	ui.tickCountLabel->setText(QString::number(info.getTickCount()));
	ui.refInputLabel->setText(QString::number(info.getReferenceInput()));
	ui.outputLabel->setText(QString::number(info.getPresentOutput()));
	ui.railVoltageLabel->setText(QString::number(info.getRailVoltage()));
	ui.currentLabel->setText(QString::number(info.getCurrent()));

	QString flagstr;
	if (!info.getValidMotor())
		flagstr.append("Invalid motor. ");
	if (!info.getHeartbeat())
		flagstr.append("Missing heartbeat. ");
	if (info.getUnderVoltage())
		flagstr.append("Under voltage. ");
	if (info.getOverCurrent())
		flagstr.append("Over current. ");
	if (!flagstr.size())
		flagstr = "Ok";

	ui.statusBar->showMessage(flagstr);
}

void MainWindow::onNewRampReference(double reference) {
	ui.rampReferenceLabel->setText(QString::number(reference*100, 'f', 2));
}

void MainWindow::onRampComplete() {
	if (ui.autoLogCheckBox->isChecked())
		onStopLogButtonClicked();
}

void MainWindow::onNewForce(double force) {
	QString forcestr = QString::number(force, 'f', 2);
	ui.forceLabel->setText(forcestr);
	ui.bangForceLabel->setText(forcestr);
	ui.loggerForceLabel->setText(forcestr);
}

void MainWindow::onSetReferenceButtonClicked() {
	motorcontroller.setReference(ui.setReferenceSpinBox->value()/100.0);
}

void MainWindow::onStopReferenceButtonClicked() {
	motorcontroller.setReference(0);
}

void MainWindow::onStartRampButtonClicked() {
	MotorRamper::Settings settings;
	settings.holdtime = ui.holdTimeSpinBox->value();
	settings.ramptime = ui.rampTimeSpinBox->value();
	settings.divisions = ui.divisionsSpinBox->value();
	settings.maxreference = ui.maxSpeedSpinBox->value()/100.0;
	settings.repeat = ui.repeatCheckbox->isChecked();
	settings.direction = (MotorRamper::Direction)ui.directionsComboBox->currentIndex();
	motorcontroller.startRamp(settings);

	if (ui.autoLogCheckBox->isChecked())
		onStartLogButtonClicked();
}

void MainWindow::onStopRampButtonClicked() {
	motorcontroller.stopRamp();

	if (ui.autoLogCheckBox->isChecked())
		onStopLogButtonClicked();
}

void MainWindow::onStartBangButtonClicked() {
	MotorBangBang::Settings settings;
	settings.holdtime = ui.bangHoldTimeSpinBox->value();
	settings.maxreference = ui.bangMaxSpeedSpinBox->value()/100.0;
	settings.random = ui.bangRandomCheckBox->isChecked();
	motorcontroller.startBangBang(settings);
}

void MainWindow::onStopBangButtonClicked() {
	motorcontroller.stopBangBang();
}

void MainWindow::onBrowseButtonClicked() {
	ui.logFileEdit->setText(QFileDialog::getSaveFileName(this));
}

void MainWindow::onConnectButtonClicked() {
	logger.connect();
	ui.loggerConnectedLabel->setText("Yes");
}

void MainWindow::onStartLogButtonClicked() {
	logger.start(ui.logFileEdit->text().toUtf8().constData());
	ui.loggingLabel->setText("Yes");
}

void MainWindow::onStopLogButtonClicked() {
	logger.stop();
	ui.loggingLabel->setText("No");
}

void MainWindow::onMagBrowseButtonClicked() {
	ui.magLogFileEdit->setText(QFileDialog::getSaveFileName(this));
}

void MainWindow::onStartMagLogButtonClicked() {
	imulogger.start(ui.magLogFileEdit->text().toUtf8().constData());
	ui.magLoggingLabel->setText("Yes");
}

void MainWindow::onStopMagLogButtonClicked() {
	imulogger.stop();
	ui.magLoggingLabel->setText("No");
}

void MainWindow::onNewBangReference(double reference) {
	ui.bangReferenceLabel->setText(QString::number(reference*100, 'f', 2));
}

void MainWindow::onNewIMU() {
	const IMUMessage &msg = imulogger.getMessage();
	ui.magXLabel->setText(QString::number(msg.mag_field[0], 'f', 4));
	ui.magYLabel->setText(QString::number(msg.mag_field[1], 'f', 4));
	ui.magZLabel->setText(QString::number(msg.mag_field[2], 'f', 4));
}


