#include "MotorCalibrate/MainWindow.h"
#include "DataObjects/MotorDriver/MotorDriverDataObjectFormatter.h"
#include "DataObjects/MotorDriver/SetReference.h"
#include "HAL/format/Sub7EPacketFormatter.h"
#include <iostream>

using namespace subjugator;
using namespace std;

MainWindow::MainWindow(int haladdr)
: motorcontroller(haladdr) {
	ui.setupUi(this);

	connect(ui.setReferenceButton, SIGNAL(clicked()), this, SLOT(onSetReferenceButtonClicked()));
	connect(ui.stopReferenceButton, SIGNAL(clicked()), this, SLOT(onStopReferenceButtonClicked()));
	connect(&motorcontroller, SIGNAL(newInfo(const MotorDriverInfo &info)), this, SLOT(onNewMotorInfo(const MotorDriverInfo &info)));
}

void MainWindow::onNewMotorInfo(const MotorDriverInfo &info) {
	ui.tickCountLabel->setText(QString::number(info.getTickCount()));
	ui.refInputLabel->setText(QString::number(info.getReferenceInput()));
	ui.outputLabel->setText(QString::number(info.getPresentOutput()));
	ui.railVoltageLabel->setText(QString::number(info.getRailVoltage()));
	ui.currentLabel->setText(QString::number(info.getCurrent()));
}

void MainWindow::onSetReferenceButtonClicked() {
	motorcontroller.setReference(ui.setReferenceSpinBox->value());
}

void MainWindow::onStopReferenceButtonClicked() {
	motorcontroller.setReference(0);
}

