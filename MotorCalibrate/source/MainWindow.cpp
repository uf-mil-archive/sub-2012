#include "MotorCalibrate/MainWindow.h"
#include "DataObjects/MotorDriver/MotorDriverDataObjectFormatter.h"
#include "HAL/format/Sub7EPacketFormatter.h"

using namespace subjugator;
using namespace std;

MainWindow::MainWindow(int haladdr)
: endpoint(hal.openDataObjectEndpoint(haladdr, new MotorDriverDataObjectFormatter(haladdr, 1, BRUSHEDOPEN), new Sub7EPacketFormatter())),
  heartbeatsender(hal.getIOService(), *endpoint) {
	ui.setupUi(this);
	hal.startIOThread();
}

