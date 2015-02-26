
#include "defines.h"

class CandidateCache
{
public:
	CandidateCache() {}

	Loc getNextMove() {
		while (_depth >= 0)
		{
			if (_currIndex[_depth] < _moveCount[_depth])
			{
				return _bigBuffer[_depth][_currIndex[_depth]++];
			}
			_depth -= 1;
		}
		return Loc::INVALID;
	}

	// Get the buffer to fill it up.
    Loc *getBuffer(Depth d) {return _bigBuffer[d]; }

	// Report how many items are used in the buffer.
	void setDepthMoves(Depth d, unsigned char moveCount) { 
		_moveCount[d] = moveCount;
		_depth = d;
	}

private:
	Loc _bigBuffer[MAX_DEPTH][MAX_CANDS];
	Breadth _moveCount[MAX_DEPTH];
	Breadth _currIndex[MAX_DEPTH];
	Depth _depth;
};

