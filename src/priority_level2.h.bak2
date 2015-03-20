#ifndef _priority_level_h
#define _priority_level_h

#include "loc.h"
#include "defines.h"

typedef short Ind;

#define MAX_COUNTS 20

class DLNode
{
public:
    DLNode()
	{
        _prevInd = 0;
        _nextInd = 0;
        _loc = -1;
        _count = 0;
	}

    void setPrevInd(Ind ind)
	{
        _prevInd = ind;
	}

    void setNextInd(Ind ind)
	{
        _nextInd = ind;
	}

    void setLoc(Loc loc)
	{
        _loc = loc;
	}

    Ind adjustCount(Ind ind)
	{
        _count += ind;
		return _count;
	}

	Ind _prevInd;
	Ind _nextInd;
	Ind _count;
	Loc _loc;
};

class PriorityLevel
{
public:
	PriorityLevel();

	Ind getCands(Loc *locBuffer, Ind max) const;
	Ind getNumCands() const;
	Ind getCount(Loc l) const;
	void addOrRemoveCandidate(Loc l, int inc=1);

private:

    Ind _headByCount[MAX_COUNTS];
    Ind _numCands = 0;
    DLNode _nodeByLoc[MAX_LOCS];
};

#endif
