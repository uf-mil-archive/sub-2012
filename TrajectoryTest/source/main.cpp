#include <QtGui/QApplication>
#include "TrajectoryTest/mainwindow.h"

using namespace subjugator;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
