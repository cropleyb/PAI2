#include "priority_level.h"
#include "bdebug.h"

PriorityLevel::PriorityLevel()
{
    _numCands = 0;

	for (int i=0; i<MAX_LOCS; i++)
	{
        _dlNodes[i]._prevInd = i-1;
        _dlNodes[i]._nextInd = i+1;
        _dlNodes[i]._loc = INVALID_LOC_VAL;
        _dlNodes[i]._count = 0;
		_dlNodes[i]._candInd = i; // for debugging only
		_nodeIndByLoc[i] = INVALID_LOC_VAL;
	}

	for (int j=0; j<MAX_COUNTS; j++)
	{
		_headByCount[j] = INVALID_LOC_VAL;
	}

	_headByCount[0] = 0;
	_dlNodes[MAX_LOCS-1].setNextInd(INVALID_LOC_VAL);
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
		while (currInd == INVALID_LOC_VAL)
		{
			// No more at this level
			currLevel--;

			// No more interesting count levels!
			if (currLevel <= 0)
				break;

			currInd = _headByCount[currLevel];
		}

		if (currInd != INVALID_LOC_VAL)
		{
			const DLNode &currNode = _dlNodes[currInd];
			locBuffer[numAdded] = currNode._loc;
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
	assert(candLocVal >= 0);
	Ind candInd = _nodeIndByLoc[candLocVal];
	BD(cout << "Entering addOrRemoveCandidate " << this << " for inc " << inc << " of loc " << (int)candLoc._value << " candInd: " << candInd << endl);

	if (candInd == INVALID_LOC_VAL)
	{
		// As yet unused loc for this priority level
		if (inc < 0)
		{
			BD(cout << "ARC 2: Trying to reduce the count below zero. Error." << endl);
			// Crash
			assert(inc == 1);
		}

		// Node for this loc has no count yet
		// Use the next node from the "free list"
		BD(cout << "ARC 4 - using node " << _headByCount[0] << endl);
		candInd = _headByCount[0];
		assert(candInd != INVALID_LOC_VAL);
		_nodeIndByLoc[candLocVal] = candInd;
		DLNode &candNode = _dlNodes[candInd];

		// Move the "free list" along
		_headByCount[0] = candNode._nextInd;
		// This is failing... The item that was put on the free list must have
		// had no _nextInd set.
		if (_headByCount[0] < 0)
		{
			BD(cout << "Free list broken for " << candNode._candInd << endl);
			assert (_headByCount[0] != INVALID_LOC_VAL);
		}

        // If we move from zero count, it must be to a count of 1.
		Ind oldHeadInd = _headByCount[1];

		// "candNode" will be the head of the "ones" list
		candNode.setPrevInd(INVALID_LOC_VAL);
		BD(cout << "setting node " << candInd << "s next ind to " << oldHeadInd << endl);
		candNode.setNextInd(oldHeadInd);
		assert(candNode._loc == INVALID_LOC_VAL);
		candNode.setLoc(candLoc);
		candNode.adjustCount(inc); // i.e. to 1
		BD(cout << "ARC 4a" << endl);

		if (oldHeadInd != INVALID_LOC_VAL)
		{
			BD(cout << "ARC 5 " << endl);
			DLNode &oldHeadNode = _dlNodes[oldHeadInd];
			oldHeadNode.setPrevInd(candInd);
		}
		// else: If this was the first candidate with this
		// count then there is no backpointer to maintain.

		// OK, put us first
		_headByCount[1] = candInd;

		BD(cout << "ARC set inc to " << inc << endl);

		_numCands += 1;
	}
	else
	{
		// This Loc already has a valid candInd for this priority level,
		// and therefore should have a valid DLNode for it.
		DLNode &candNode = _dlNodes[candInd];
		if (candNode._loc != candLoc)
		{
			cout << "Expected loc: " << candLoc << "(" << candLoc._value << ") got " << candNode._loc << "(" << candNode._loc._value << ")" << endl;
		}
		assert(candNode._loc == candLoc);

		// Remove from existing count structure list
		if (candNode._prevInd != INVALID_LOC_VAL) {
			// Not the first for that count level
			DLNode &prev = _dlNodes[candNode._prevInd];
			BD(cout << "Update " << candInd << "'s prev's next field (" << candNode._prevInd << ") to " << candNode._nextInd << endl);
#if 0
			Entering addOrRemoveCandidate 0x10f281e8c for -1 527
			Update 14's next field of 4 to 15
			Entering addOrRemoveCandidate 0x10f281e8c for -1 593
			Update 15's next field of 4 to -1
			Overwriting next value that was just set?!
			Why did we get two "4"s? candNode._prevInd
#endif
			prev.setNextInd(candNode._nextInd);
		}
		if (candNode._nextInd != INVALID_LOC_VAL) {
			// Not the last for that count level
			DLNode &next = _dlNodes[candNode._nextInd];
			next._prevInd = candNode._prevInd;
		}
		
		// Remove from count structure heads if necessary
		if (_headByCount[candNode._count] == candInd)
		{
			_headByCount[candNode._count] = candNode._nextInd;
			if (candNode._count == 0)
				assert(candNode._nextInd != INVALID_LOC_VAL);
		}

		candNode._count += inc;

		// Maintain a count of the total number of candidates
		// at this priority level, not including duplicates (count > 0)
		if (candNode._count == 0)
		{
			_numCands--;
			//candNode._loc = INVALID_LOC_VAL;
			_nodeIndByLoc[candLocVal] = INVALID_LOC_VAL;
			candNode.setLoc(INVALID_LOC_VAL);
		}

		// Add to appropriate count structure head
		Ind oldHead = _headByCount[candNode._count];
		assert(candNode._count || oldHead != INVALID_LOC_VAL);
		
		// We're going at the front of that list
		candNode.setNextInd(oldHead);
		candNode._prevInd = INVALID_LOC_VAL;

		if (oldHead != INVALID_LOC_VAL)
		{
			// Update the previous head
			_dlNodes[oldHead]._prevInd = candInd;
		}

		// Put us at the front of the list
		_headByCount[candNode._count] = candInd;
	}
}

Ind PriorityLevel::getCount(Loc l) const
{
	Ind nodeInd = _nodeIndByLoc[l._value];
	if (l._value == INVALID_LOC_VAL || nodeInd == INVALID_LOC_VAL)
		return 0;
	return _dlNodes[nodeInd]._count;
}

