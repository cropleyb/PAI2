
#include "defines.h"

class CandidateCache
{
public:
	CandidateCache() {}

	Loc getNextMove(Depth depth) {
		Breadth currInd = _currIndex[depth];
		if (currInd < _moveCount[depth])
		{
			// Move on...
			_currIndex[depth]++;
			return _bigBuffer[depth][currInd];
		}
		// No more left at current depth
		return Loc::INVALID;
	}

	// Get the buffer to fill it up.
    Loc *getBuffer(Depth d) {return _bigBuffer[d]; }

	// Report how many items are used in the buffer, and reset the "iterator"
	void setDepthMoves(Depth d, unsigned char moveCount) { 
		_moveCount[d] = moveCount;
		_currIndex[d] = 0;
	}

private:
	Loc _bigBuffer[MAX_DEPTH][MAX_CANDS];
	Breadth _moveCount[MAX_DEPTH];
	Breadth _currIndex[MAX_DEPTH];
};

