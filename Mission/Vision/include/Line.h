#ifndef LINE_H
#define LINE_H

#include <vector>

#include "AvgLine.h"
#include "IOImages.h"

class Line
{
public:
	std::vector<AvgLine> avgLines;
	Line(int num);
	int findLines(IOImages* ioimages);
	void drawResult(IOImages* ioimages, int objectID);

private:
	int numberOfLinesToFind;

};

#endif
