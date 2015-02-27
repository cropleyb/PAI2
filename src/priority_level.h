#include "loc.h"
#include "defines.h"

typedef short Ind;

// #define MAX_NODES 19*19

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
	void addOrRemoveCandidate(Loc l, int inc=1);

private:
    Ind _freeListInd = 0;
    Ind _dlHeadInd = -1;
    Ind _numCands = 0;
    DLNode _dlNodes[MAX_LOCS];
    Ind _nodeIndByLoc[MAX_LOCS];
};

