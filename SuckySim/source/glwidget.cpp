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
    // Add Buoys
    boost::shared_ptr<SimBuoy> simBuoy = boost::shared_ptr<SimBuoy>(new SimBuoy("BuoyRed",8, Qt::red));
    simBuoy->setPosition_NED(Vector3d(5,5,0));

    boost::shared_ptr<SimSub> simSub = boost::shared_ptr<SimSub>(new SimSub("SimSub1",8, Qt::black));
    simSub->setPosition_NED(Vector3d(5,5,0));
    simSub->setRPY(Vector3d(0,0,(0* boost::math::constants::pi<double>()/180)));

    world.AddObjectToWorld(simBuoy);
    world.AddObjectToWorld(simSub);
   // world.AddObjectToWor(((length-podLength)/2)*pPerM)ld(boost::shared_ptr<SimBuoy>(new SimBuoy("BuoyRed",8, Qt::red)));
   // world.AddObjectToWorld(boost::shared_ptr<SimPipe>(new SimPipe("Pipe",8, Qt::red)));
   // world.AddObjectToWorld(boost::shared_ptr<SimHydrophone>(new SimHydrophone("HPhone27k", 0, 270000)));

    world.Draw(&painter);

    painter.end();
}
