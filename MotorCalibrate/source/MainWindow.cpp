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
	connect(&motorcontroller, SIGNAL(newInfo()), this, SLOT(onNewMotorInfo()));
	connect(&motorcontroller, SIGNAL(newRampReference(double)), this, SLOT(onNewRampReference(double)));
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
	settings.maxreference = 1;
	settings.repeat = ui.repeatCheckbox->isChecked();

	if (ui.logCheckBox->isChecked()) {
		QString filename = QFileDialog::getSaveFileName(this);
		logger.start(filename.toUtf8().constData());
	}

	motorcontroller.startRamp(settings);
}

void MainWindow::onStopRampButtonClicked() {
	logger.stop();
	motorcontroller.stopRamp();
}

