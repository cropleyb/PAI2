#include "loc.h"

typedef unsigned short Ind;

#define MAX_NODES 19*19

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

	int copyCands(Loc *candidateBuffer);
	void addOrRemoveCandidate(Loc l, int inc=1);

private:
    Ind _freeListInd = 0;
    Ind _dlHeadInd = -1;
    Ind _numCands = 0;
    DLNode _dlNodes[MAX_NODES];
    Ind _nodeIndByLoc[MAX_NODES];
};

