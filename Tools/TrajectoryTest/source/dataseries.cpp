#include "TrajectoryTest/dataseries.h"
#include <math.h>
#include <QDebug>

using namespace subjugator;

DataSeries::DataSeries(double interval, size_t numPoints):
    d_y(NULL),
    d_referenceTime(0.0),
    d_startIndex(0),
    d_offset(0.0),
    d_numPoints(0)
{
    fill(interval, numPoints);
}

// Size and start the
void DataSeries::fill(double interval, size_t numPoints)
{
    d_numPoints = numPoints;

    if ( interval <= 0.0 || numPoints < 2 )
        return;

    points temp = {0,0};

    d_values.resize(numPoints);
    d_values.fill(temp);
    d_step = 1;

    d_interval = interval;
}

// Update function to gather data from a defined function.
void DataSeries::update(int size)
{
	d_values.empty();

    for ( int i = 0; i < size; i++ )
        addXY(d_y(i, Component));
}

// Add points to the vector holding the curves points.
void DataSeries::addXY(points pos)
{
    d_values.append(pos);
    d_values.remove(1,1);
}

// Set function for curve to pull points from.
void DataSeries::setFunction(points(*y)(int, char))
{
    d_y = y;
}

// Set component for curve to pull points from.
void DataSeries::setComponent(char c)
{
    Component = c;
}

void DataSeries::setReferenceTime(double timeStamp)
{
    d_referenceTime = timeStamp;

    const double startTime = ::fmod(d_referenceTime, d_values.size() * d_step);

    d_startIndex = int(startTime / d_step); // floor
    d_offset = ::fmod(startTime, d_step);
}

double DataSeries::referenceTime() const
{
    return d_referenceTime;
}

size_t DataSeries::size() const
{
    return d_values.size();
}

// Set all points for the curves Data Series
QPointF DataSeries::sample(size_t i) const
{
    const int size = d_values.size();

    int index = d_startIndex + i;
    if ( index >= size )
        index -= size;

    const double x = d_values.data()[index].x * d_step - d_offset;//- d_interval;  // Shift Data
    const double y = d_values.data()[index].y;

    //qDebug() << "X: " << x << endl << "Y: " << y;

    return QPointF(x, y);
}

QRectF DataSeries::boundingRect() const
{
    return QRectF(-1.0, -d_interval, 2.0, d_interval);
}
