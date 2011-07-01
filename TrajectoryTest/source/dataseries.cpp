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

void DataSeries::fill(double interval, size_t numPoints)
{
    d_numPoints = numPoints;

    if ( interval <= 0.0 || numPoints < 2 )
        return;

    d_values.resize(numPoints);
//    d_values.fill(0.0);
//
//    if ( d_y )
//    {
//        d_step = 40.0 / (numPoints - 2);
//        for ( size_t i = 0; i < numPoints; i++ )
//            d_values[i] = d_y(i * d_step);
//    }
    d_step = 1;

    d_interval = interval;
}

void DataSeries::addData(double val)
{
    d_values.append(val);

    qDebug() << "Size after append: " << d_values.size();

    if (d_values.size() > d_numPoints)
       d_values.remove(1,1);

    qDebug() << "Size after remove: " << d_values.size();
}

void DataSeries::update(int size)
{
    for ( int i = 0; i < size; i++ )
        addXY(d_y(i));
}

void DataSeries::addXY(points pos)
{
    qDebug() << "X: " << pos.x << endl << "Y: " << pos.y;

    dataPts.append(pos);

    //dataPts.remove(1,1);
}

void DataSeries::setFunction(points(*y)(int))
{
    d_y = y;
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

QPointF DataSeries::sample(size_t i) const
{
    const int size = d_values.size();

    int index = d_startIndex + i;
    if ( index >= size )
        index -= size;

    //qDebug() << "DSTEP: " << d_step;

    const double x = i * d_step - d_offset - d_interval;
    const double y = d_values.data()[index];

    return QPointF(x, y);
}

QRectF DataSeries::boundingRect() const
{
    return QRectF(-1.0, -d_interval, 2.0, d_interval);
}
