#include "MotorCalibrate/MainWindow.h"
#include "DataObjects/MotorDriver/MotorDriverDataObjectFormatter.h"
#include "DataObjects/MotorDriver/SetReference.h"
#include "HAL/format/Sub7EPacketFormatter.h"
#include <iostream>

using namespace subjugator;
using namespace std;

MainWindow::MainWindow(int haladdr)
: endpoint(hal.openDataObjectEndpoint(haladdr, new MotorDriverDataObjectFormatter(haladdr, 1, BRUSHEDOPEN), new Sub7EPacketFormatter())),
  heartbeatsender(hal.getIOService(), *endpoint) {
	ui.setupUi(this);
	hal.startIOThread();

	connect(ui.setReferenceButton, SIGNAL(clicked()), this, SLOT(onSetReferenceButtonClicked()));
	connect(ui.stopReferenceButton, SIGNAL(clicked()), this, SLOT(onStopReferenceButtonClicked()));
}

void MainWindow::onSetReferenceButtonClicked() {
	endpoint->write(SetReference(ui.setReferenceSpinBox->value()));
}

void MainWindow::onStopReferenceButtonClicked() {
	endpoint->write(SetReference(0));
}

