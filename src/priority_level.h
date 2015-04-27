#ifndef _priority_level_h
#define _priority_level_h

#include "loc.h"
#include "defines.h"

#include <array>

typedef short Ind;

// #define MAX_NODES 19*19

#include <string>
using namespace std;

class DLNode
{
public:
    DLNode() {
        _prevInd = 0;
        _nextInd = 0;
        _loc = -1;
        _count = 0;
	}

    void setPrevInd(Ind ind) {
        _prevInd = ind;
	}

    void setNextInd(Ind ind) {
        _nextInd = ind;
	}

    void setLoc(Loc loc) {
        _loc = loc;
	}

    Ind adjustCount(Ind ind) {
        _count += ind;
		assert(_count >= 0);
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

	void reset();

	Ind getCands(Loc *locBuffer, Ind max, U64 seen[MAX_WIDTH]) const;

	// The number of candidate move locations that extend at least one line
	// that is currently at this priority level.
	// For capture and take PLs, this is the number of unique candidate move 
	// locations that provide at least one capture or threat possibility.
	Ind getNumCands() const;

	// Get up to three of the above candidates have a count of 2 or more lines.
	Breadth getMultiCands(Loc multiCands[3]) const;

	// Get the count of times that the given location provides this PL's
	// advantage.
	Ind getCount(Loc l) const;

	// Maintain our complex data structure incrementally for changes to a loc.
	void addOrRemoveCandidate(Loc l, int inc);

	void setLevelName(string val) { _levelName = val; }
	string getLevelName() const { return _levelName; }

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

	// Debugging only
	string _levelName;
};

#endif
