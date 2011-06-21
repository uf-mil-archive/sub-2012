#include "DepthTest/depthtest.h"
#include "DataObjects/Depth/DepthDataObjectFormatter.h"
#include "HAL/format/Sub7EPacketFormatter.h"
#include <iostream>
#include <QFileDialog>

using namespace subjugator;
using namespace std;

MainWindow::MainWindow(int haladdr)
: depthcontroller(haladdr),
  logger(depthcontroller, "/dev/ttyUSB1") {
	ui.setupUi(this);   
	ui.statusBar->showMessage("No connection to depth board");

	connect(ui.startLogButton, SIGNAL(clicked()), this, SLOT(onStartLogButtonClicked()));
	connect(ui.stopLogButton, SIGNAL(clicked()), this, SLOT(onStopLogButtonClicked()));
	connect(ui.browseButton, SIGNAL(clicked()), this, SLOT(onBrowseButtonClicked()));
	connect(&depthcontroller, SIGNAL(newInfo()), this, SLOT(onNewDepthInfo()));
	
}

void MainWindow::onNewDepthInfo() {
	const DepthInfo &info = depthcontroller.getDepthInfo();
	ui.tickCountLabel->setText(QString::number(info.getTickCount()));
	ui.depthLabel->setText(QString::number(info.getDepth()));

	QString flagstr;
	if (!flagstr.size())
		flagstr = "Ok";

	ui.statusBar->showMessage(flagstr);
}


void MainWindow::onBrowseButtonClicked() {
	ui.logFileEdit->setText(QFileDialog::getSaveFileName(this));
}

void MainWindow::onStartLogButtonClicked() {
	logger.start(ui.logFileEdit->text().toUtf8().constData());
	ui.loggingLabel->setText("Yes");
}

void MainWindow::onStopLogButtonClicked() {
	logger.stop();
	ui.loggingLabel->setText("No");
}
