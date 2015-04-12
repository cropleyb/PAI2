#include <array>

#include "defines.h"

class CandidateCache
{
public:
	CandidateCache()
	{
		reset();
	}

	void reset()
	{
		_lastDepthRequested = -1;
		for (Breadth b=0; b<MAX_DEPTH; b++)
		{
			_moveCount[b] = 0;
			_currIndex[b] = 0;
		}
	}

	Loc getNextMove(Depth depth) {
		Breadth currInd = _currIndex[depth];
		_lastDepthRequested = depth;
		if (currInd < _moveCount[depth])
		{
			// Move on...
			_currIndex[depth]++;
			return _bigBuffer[depth][currInd];
		}
		_moveCount[depth] = 0;
		// No more left at current depth
		return Loc::INVALID;
	}

	// Get the buffer to fill it up.
    Loc *getBuffer(Depth d) {return _bigBuffer[d]; }

	// Report how many items are used in the buffer, and reset the "iterator"
	void setDepthMoves(Depth d, unsigned char moveCount) { 
		_lastDepthRequested = d;
		_moveCount[d] += moveCount;
		_currIndex[d] = 0;
	}

	bool needsFilling(Depth d)
	{
		return (d > _lastDepthRequested);
	}

	Breadth getNumMoves(Depth d) const
	{
		return _moveCount[d];
	}

private:
	Loc _bigBuffer[MAX_DEPTH][MAX_CANDS];
	//std::array<std::array<Loc, MAX_DEPTH>, MAX_CANDS> _bigBuffer; // TODO
	std::array<Breadth, MAX_DEPTH> _moveCount;
	std::array<Breadth, MAX_DEPTH> _currIndex;
	Depth _lastDepthRequested;
};

