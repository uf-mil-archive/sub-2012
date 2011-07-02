#ifndef DATASERIES_H
#define DATASERIES_H

#include <qwt_series_data.h>
#include <qvector.h>

struct points{
    double x;
    double y;
};

namespace subjugator
{
	class DataSeries: public QwtSeriesData<QPointF>
	{
	public:
		DataSeries(double interval = 10.0, size_t numPoints = 1000);
		void fill(double interval, size_t numPoints);
		void addData(double val);
		void addXY(points pos);
	    void update(int size);

		void setReferenceTime(double);
		double referenceTime() const;

		virtual size_t size() const;
		virtual QPointF sample(size_t i) const;

		virtual QRectF boundingRect() const;

		void setFunction(points(*y)(int));

		QVector<points> dataPts;

	private:
		points (*d_y)(int);

		double d_referenceTime;
		double d_interval;
		QVector<double> d_values;

		double d_step;
		int d_startIndex;
		double d_offset;
		int d_numPoints;
	};
}
#endif // DATASERIES_H
