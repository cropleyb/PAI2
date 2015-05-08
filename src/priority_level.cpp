#include "priority_level.h"
#include "pente_game.h"
#include "bdebug.h"

#include <algorithm>    // std::sort
#include <vector>       // std::vector

//#define PLDEBUG

#ifdef PLDEBUG 
//PLD is very time consuming
#include <iostream>
using namespace std;
#define PLD(X) X
#else
#define PLD(X)
#endif

PriorityLevel::PriorityLevel()
{
	reset();
}

void PriorityLevel::reset()
{
    _freeListInd = 0;
    _dlHeadInd = -1;
    _numCands = 0;

	for (int i=0; i<MAX_LOCS; i++)
	{
        _dlNodes[i].setNextInd(i+1);
		_dlNodes[i]._prevInd = i-1;
		_dlNodes[i]._count = 0;
		_dlNodes[i]._loc = Loc::INVALID;
		_nodeIndByLoc[i] = -1;
	}

	_dlNodes[MAX_LOCS-1].setNextInd(-1);
}

void PriorityLevel::addOrRemoveCandidate(Loc candLoc, int inc)
{
	PLD(cout << "ARC: " << (void *)this << "----------------" << endl);

	// Get the index of the node for the given candidate location
	CompressedLoc candLocVal = candLoc._value;
	Ind nodeInd = _nodeIndByLoc[candLocVal];

	if (nodeInd < 0)
	{
		PLD(cout << "ARC 1" << endl);
		// assert(inc > 0);

		// Node for this loc has no count yet
		// Use the next node from the free list
		PLD(cout << "ARC 4 - using node " << _freeListInd << endl);
		nodeInd = _freeListInd;
		_nodeIndByLoc[candLocVal] = nodeInd;
		DLNode &node = _dlNodes[nodeInd];
		_freeListInd = node._nextInd;

		Ind nextInd = _dlHeadInd;
		Ind oldHeadInd = _dlHeadInd;
		node.setPrevInd(-1); // "node" is currently the head of the list
		node.setNextInd(nextInd);
		PLD(cout << "ARC 4a" << endl);

		if (oldHeadInd >= 0)
		{
			PLD(cout << "ARC 5 " << endl);
			DLNode &oldHead = _dlNodes[oldHeadInd];
			oldHead.setPrevInd(nodeInd);
		}

		PLD(cout << "ARC set head ind to ---------------" << nodeInd << endl);
		_dlHeadInd = nodeInd;
		PLD(cout << "ARC set node loc to ---------------" << candLoc._value << endl);
		node.setLoc(candLoc);
		PLD(cout << "ARC set inc to " << inc << endl);
		node.adjustCount(inc);

		_numCands += 1;
	}
	else
	{
		PLD(cout << "ARC B 1" << endl);
		// A node for this loc has already been used, update its count
		DLNode &node = _dlNodes[nodeInd];
		Ind newCount = node.adjustCount(inc);

		if (newCount <= 0)
		{
			// We don't need this node any more
			// assert(newCount == 0);

			// Remove it from its current place in the dlList
			Ind prevNodeInd = node._prevInd;
			Ind nextNodeInd = node._nextInd;

			if (prevNodeInd >= 0)
			{
				DLNode &prevNode = _dlNodes[prevNodeInd];
				prevNode.setNextInd(nextNodeInd);
			}

			if (nextNodeInd >= 0)
			{
				DLNode &nextNode = _dlNodes[nextNodeInd];
				nextNode.setPrevInd(prevNodeInd);
			}

			if (_dlHeadInd == nodeInd)
			{
				Ind newHeadInd = node._nextInd;
				PLD(cout << "addOrRemoveCandidate: setting headInd to " << newHeadInd << endl);
				_dlHeadInd = newHeadInd;
			}

			_nodeIndByLoc[candLocVal] = -1;

			// TODO Put it at the head of the free list
			node.setNextInd(_freeListInd);
			_freeListInd = nodeInd;

			_numCands -= 1;
		}
	}
}

Ind PriorityLevel::getNumCands() const
{
#if 0
	// TEMP
	if (_numCands > 0) {
		const DLNode &headNode = _dlNodes[_dlHeadInd];
		assert(headNode._count);
	}
	// PRODUCTION
#endif
	return _numCands;
}

Ind PriorityLevel::getCount(Loc l) const
{
	Ind nodeInd = _nodeIndByLoc[l._value];
	const DLNode &node = _dlNodes[nodeInd];
	return node._count;
}

bool PriorityLevel::myOrder(const Loc &l1, const Loc &l2) const
{
	// const DLNode &currNode = _dlNodes[currInd];

	return (getCount(l1) > getCount(l2));
}

Ind PriorityLevel::getCands(Loc *locBuffer, Ind requestedMax, U64 seen[MAX_WIDTH]) const
{
	PLD(cout << "getCands top - " << (void *)this << endl);
	PLD(cout << "getCands 1 - _dlHeadInd" << _dlHeadInd << endl);
	Ind numAdded = 0;
	Ind currInd = _dlHeadInd;

	// TODO: Bail if only request 1 or 2?
	
	Ind tempMax = requestedMax;
	if (_numCands > requestedMax)
	{
		// This is the last priority level that will be used to contribute
		// to the candidate moves. Get all of them so we can sort them by
		// decreasing _count.
		tempMax += 100;
		if (tempMax > _numCands) tempMax = _numCands;
	}

	while ((currInd >= 0) && (numAdded < tempMax))
	{
		PLD(cout << "getCands 2 - currInd: " << currInd << endl);
		const DLNode &currNode = _dlNodes[currInd];
		if (currNode._loc == Loc::INVALID)
		{
			PLD(cout << "getCands 3 - INVALID" << endl);
			break;
		}
		assert(currNode._count > 0);
		Coord currNodeX = currNode._loc[0];
		Coord currNodeY = currNode._loc[1];
		if (!(seen[currNodeY] & ((U64)1 << currNodeX)))
		{
			PLD(cout << "getCands 4 - adding " << currNode._loc._value << endl);
			seen[currNodeY] |= ((U64)1 << currNodeX);

			// The _penteGame_ global is set at the start of every search.
			if (_penteGame_ && !_penteGame_->isLegalMove(currNode._loc)) continue;
			locBuffer[numAdded] = currNode._loc;
			numAdded++;
		} else {
			PLD(cout << "getCands not adding as previously 'seen': " << currNode._loc << endl;)
		}

		currInd = currNode._nextInd;
	}
	PLD(cout << "getCands 5" << endl);

	// Sort the candidates by decreasing _count at this priority level.
	// A more frequent number of occurrences is likely to mean a better move.
	// Note that if there are more candidates at this level than are requested
	// by the search, only the best candidates are used.
    std::sort(locBuffer, locBuffer+numAdded, doCompare(*this) );

#if 0
	if (_numCands > requestedMax)
	{
		int lastReqCount = _dlNodes[_nodeIndByLoc[locBuffer[requestedMax-1]._value]]._count;
		while (requestedMax < numAdded)
		{
			int extraCount = _dlNodes[_nodeIndByLoc[locBuffer[requestedMax]._value]]._count;
			if (extraCount == lastReqCount) requestedMax++;
			else break;
		}
	}
#endif

	return std::min(requestedMax, numAdded);
}

// Get up to three of the above candidates that have a count of 2 or more lines.
Breadth PriorityLevel::getMultiCands(Loc multiCands[3]) const
{
	PLD(cout << "getMultiCands top - " << (void *)this << endl);
	PLD(cout << "getMultiCands 1 - _dlHeadInd" << _dlHeadInd << endl);
	Ind currInd = _dlHeadInd;
    Ind multis = 0;

	while ((currInd >= 0) && (multis < 3))
	{
		PLD(cout << "getMultiCands 2 - currInd: " << currInd << endl);
		const DLNode &currNode = _dlNodes[currInd];
		if (currNode._loc == Loc::INVALID)
		{
			PLD(cout << "getMultiCands 3 - INVALID" << endl);
			break;
		}
		if (currNode._count > 1)
		{
			PLD(cout << "getMultiCands 4 - found one" << endl);
			multiCands[multis++] = currNode._loc;
			if (multis >= 3) return multis;
		}
		currInd = currNode._nextInd;
	}
	return multis;
}
