#ifndef LINE_H
#define LINE_H

#include "MILObjectIDs.h"
#include "IOImages.h"
#include <vector>
#include "AvgLine.h"

class Line
{
public:
	vector<AvgLine> avgLines;
	Line(int num);
	~Line(void);
	int findLines(IOImages* ioimages);
	void drawResult(IOImages* ioimages, int objectID);

private:
	int numberOfLinesToFind;
	Mat edgeImage;
	vector<Vec4i> lines;	
	double tmpAngle;
	
	

};

#endif
