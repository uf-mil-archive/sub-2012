#ifndef FINDER_RESULT_H
#define FINDER_RESULT_H

class FinderResult
{
public:
	FinderResult() : objectID(0), u(0), v(0), scale(0), angle(0) { }
	FinderResult(int objectID, int u, int v, double scale, double angle) : objectID(objectID), u(u), v(v), scale(scale), angle(angle) { }

	int objectID;
	int u;
	int v;
	double scale;
	double angle;
};

#endif
