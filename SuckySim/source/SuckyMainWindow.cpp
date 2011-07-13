#include <QtGui>
#include "SuckySim/glwidget.h"
#include "SuckySim/SuckyMainWindow.h"

using namespace subjugator;

SuckyMainWindow::SuckyMainWindow()
    : QWidget()
{
    GLWidget *openGL = new GLWidget(this);
    QLabel *openGLLabel = new QLabel(tr("OpenGL"));
    openGLLabel->setAlignment(Qt::AlignHCenter);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(openGL, 0, 0);
    layout->addWidget(openGLLabel, 1, 0);
    setLayout(layout);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), openGL, SLOT(animate()));
    timer->start(50);

    setWindowTitle(tr("2D Painting on Native and OpenGL Widgets"));
}
//! [0]
