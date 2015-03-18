#include "priority_level.h"
#include "bdebug.h"

PriorityLevel::PriorityLevel()
{
    _numCands = 0;

	for (int i=0; i<MAX_LOCS; i++)
	{
        _nodeByLoc[i]._prevInd = i-1;
        _nodeByLoc[i]._nextInd = i+1;
        _nodeByLoc[i]._loc = i;
        _nodeByLoc[i]._count = 0;
	}

	for (int j=0; j<MAX_COUNTS; j++)
	{
		_headByCount[j] = -1;
	}

	_headByCount[0] = 0;
}

Ind PriorityLevel::getNumCands() const
{
	return _numCands;
}

Ind PriorityLevel::getCands(Loc *locBuffer, Ind max) const
{
	Ind numAdded = 0;

	Ind currLevel = MAX_COUNTS - 1; // TODO: cache highest level seen
	Ind currInd = _headByCount[currLevel];

	while ((currLevel > 0) && (numAdded < max))
	{
		while (currInd < 0)
		{
			// No more at this level
			currLevel--;

			// No more interesting count levels!
			if (currLevel <= 0)
				break;

			currInd = _headByCount[currLevel];
		}

		if (currInd >= 0)
		{
			const DLNode &currNode = _nodeByLoc[currInd];
			locBuffer[numAdded] = currInd;
			currInd = currNode._nextInd;
			numAdded++;
		}
	}
	return numAdded;
}

void PriorityLevel::addOrRemoveCandidate(Loc candLoc, int inc)
{
	// Get the index of the node for the given candidate location
	CompressedLoc candLocVal = candLoc._value;

	DLNode &node = _nodeByLoc[candLocVal];

	// Remove from existing count structure list
	if (node._prevInd >= 0) {
		DLNode &prev = _nodeByLoc[node._prevInd];
		prev._nextInd = node._nextInd;
	}
	if (node._nextInd >= 0) {
		DLNode &next = _nodeByLoc[node._nextInd];
		next._prevInd = node._prevInd;
	}
	
	// Remove from count structure heads if necessary
	if (_headByCount[node._count] == candLocVal)
	{
		_headByCount[node._count] = node._nextInd;
	}
	// I am trying to maintain a count of the total number of candidates
	// at this priority level, not including duplicates (count > 0)
	if (node._count + inc == 0)
		_numCands--;

	node._count += inc;

	if (node._count == 1 && inc > 0)
		_numCands++;

	// Add to count structure heads
	Ind oldHead = _headByCount[node._count];
	
	// We're going at the front of that list
	node._nextInd = oldHead;

	if (oldHead >= 0)
	{
		// Update the previous head
		_nodeByLoc[oldHead]._prevInd = candLocVal;
	}

	// Put us at the front of the list
	_headByCount[node._count] = candLocVal;
}

Ind PriorityLevel::getCount(Loc l) const
{
	return _nodeByLoc[l._value]._count;
}

