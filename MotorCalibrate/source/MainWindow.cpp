#include "MotorCalibrate/MainWindow.h"
#include "DataObjects/MotorDriver/MotorDriverDataObjectFormatter.h"
#include "DataObjects/MotorDriver/SetReference.h"
#include "HAL/format/Sub7EPacketFormatter.h"
#include <iostream>
#include <QFileDialog>

using namespace subjugator;
using namespace std;

MainWindow::MainWindow(int haladdr)
: motorcontroller(haladdr),
  logger(motorcontroller, "/dev/ttyUSB0") {
	ui.setupUi(this);

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
}

void MainWindow::onNewMotorInfo() {
	const MotorDriverInfo &info = motorcontroller.getMotorInfo();
	ui.tickCountLabel->setText(QString::number(info.getTickCount()));
	ui.refInputLabel->setText(QString::number(info.getReferenceInput()));
	ui.outputLabel->setText(QString::number(info.getPresentOutput()));
	ui.railVoltageLabel->setText(QString::number(info.getRailVoltage()));
	ui.currentLabel->setText(QString::number(info.getCurrent()));
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

void MainWindow::onNewBangReference(double reference) {
	ui.bangReferenceLabel->setText(QString::number(reference*100, 'f', 2));
}

