#include "MotorCalibrate/MainWindow.h"
#include <QApplication>
#include <sstream>

using namespace subjugator;
using namespace std;

int main(int argc, char **argv) {
	QApplication app(argc, argv);

	int motaddr=30;
	if (argc > 1) {
		stringstream buf(argv[1]);
		buf >> motaddr;
	}

	MainWindow mainwindow(motaddr);
	mainwindow.show();

	return app.exec();
}

