#ifndef _priority_level_h
#define _priority_level_h

#include "loc.h"
#include "defines.h"

#include <array>

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

	Ind getCands(Loc *locBuffer, Ind max, U64 seen[MAX_WIDTH]) const;
	Ind getNumCands() const;
	Ind getCount(Loc l) const;
	void addOrRemoveCandidate(Loc l, int inc=1);

private:
	bool myOrder(const Loc &l1, const Loc &l2) const;

    struct doCompare
    { 
        doCompare( const PriorityLevel& info ) : _mInfo(info) { } // only if you really need the object state
        const PriorityLevel& _mInfo;

        bool operator()( const Loc& i1, const Loc& i2  )
        { 
            // comparison code using m_info
            int count1 = _mInfo._dlNodes[_mInfo._nodeIndByLoc[i1._value]]._count;
            int count2 = _mInfo._dlNodes[_mInfo._nodeIndByLoc[i2._value]]._count;
			return count1 > count2;
        }
    };

    Ind _freeListInd = 0;
    Ind _dlHeadInd = -1;
    Ind _numCands = 0;
	std::array<DLNode, MAX_LOCS> _dlNodes;
	std::array<Ind, MAX_LOCS> _nodeIndByLoc;
};

#endif
