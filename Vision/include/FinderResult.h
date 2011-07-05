#ifndef FINDER_RESULT_H
#define FINDER_RESULT_H

class FinderResult
{
public:
	int objectID;
	double timestamp;

	FinderResult(void);
	virtual ~FinderResult(void);
};

#endif
