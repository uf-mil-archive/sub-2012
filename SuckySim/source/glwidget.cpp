#include <QtGui>
#include "SuckySim/glwidget.h"

using namespace subjugator;

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    setFixedSize(480, 480);
    setAutoFillBackground(false);
}

void GLWidget::animate()
{
    repaint();
}

void GLWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(event->rect(), Qt::cyan);

    SimWorld world("world", 10, 10, 10, 480, 480);
   // world.AddObjectToWorld(boost::shared_ptr<SimBuoy>(new SimBuoy("BuoyRed",8, Qt::red)));
   // world.AddObjectToWorld(boost::shared_ptr<SimPipe>(new SimPipe("Pipe",8, Qt::red)));
    world.AddObjectToWorld(boost::shared_ptr<SimHydrophone>(new SimHydrophone("HPhone27k", 0, 270000)));

    world.Draw(&painter);

    painter.end();
}
