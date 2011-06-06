#include "MotorCalibrate/MainWindow.h"
#include <QApplication>

using namespace subjugator;
using namespace std;

int main(int argc, char **argv) {
	QApplication app(argc, argv);

	MainWindow mainwindow;
	mainwindow.show();

	return app.exec();
}

