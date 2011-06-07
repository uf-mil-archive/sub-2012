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
  logger(motorcontroller, "/dev/ttyUSB1") {
	ui.setupUi(this);

	connect(ui.setReferenceButton, SIGNAL(clicked()), this, SLOT(onSetReferenceButtonClicked()));
	connect(ui.stopReferenceButton, SIGNAL(clicked()), this, SLOT(onStopReferenceButtonClicked()));
	connect(ui.startRampButton, SIGNAL(clicked()), this, SLOT(onStartRampButtonClicked()));
	connect(ui.stopRampButton, SIGNAL(clicked()), this, SLOT(onStopRampButtonClicked()));
	connect(ui.startBangButton, SIGNAL(clicked()), this, SLOT(onStartBangButtonClicked()));
	connect(ui.stopBangButton, SIGNAL(clicked()), this, SLOT(onStopBangButtonClicked()));
	connect(&motorcontroller, SIGNAL(newInfo()), this, SLOT(onNewMotorInfo()));
	connect(&motorcontroller, SIGNAL(newRampReference(double)), this, SLOT(onNewRampReference(double)));
	connect(&motorcontroller, SIGNAL(newBangReference(double)), this, SLOT(onNewBangReference(double)));
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

void MainWindow::onNewForce(double force) {
	ui.forceLabel->setText(QString::number(force, 'f', 2));
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
}

void MainWindow::onStopRampButtonClicked() {
	motorcontroller.stopRamp();
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

void MainWindow::onNewBangReference(double reference) {
	ui.bangReferenceLabel->setText(QString::number(reference*100, 'f', 2));
}

