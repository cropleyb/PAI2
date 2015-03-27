#include "priority_level.h"
#include "bdebug.h"

#include <algorithm>    // std::sort
#include <vector>       // std::vector

#define PLD(X)

PriorityLevel::PriorityLevel()
{
	//PLD(cout << "PriorityLevel constructor" << this << "----------------" << endl);
    _freeListInd = 0;
    _dlHeadInd = -1;
    _numCands = 0;

	for (int i=0; i<MAX_LOCS; i++)
	{
        _dlNodes[i].setNextInd(i+1);
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

Ind PriorityLevel::getCands(Loc *locBuffer, Ind max, U64 seen[MAX_WIDTH]) const
{
	PLD(cout << "getCands top - " << (void *)this << endl);
	PLD(cout << "getCands 1 - _dlHeadInd" << _dlHeadInd << endl);
	Ind numAdded = 0;
	Ind currInd = _dlHeadInd;

	// TODO: static heap of size MAX_CANDS

	while ((currInd >= 0) && (numAdded < max))
	{
		PLD(cout << "getCands 2 - currInd: " << currInd << endl);
		const DLNode &currNode = _dlNodes[currInd];
		if (currNode._loc == Loc::INVALID)
		{
			PLD(cout << "getCands 3 - INVALID" << endl);
			break;
		}
		Coord currNodeX = currNode._loc[0];
		Coord currNodeY = currNode._loc[1];
		if (!(seen[currNodeY] & ((U64)1 << currNodeX)))
		{
			PLD(cout << "getCands 4 - adding " << currNode._loc._value << endl);
			locBuffer[numAdded] = currNode._loc;
			seen[currNodeY] |= ((U64)1 << currNodeX);
			numAdded++;
		}
		currInd = currNode._nextInd;
	}
	PLD(cout << "getCands 5" << endl);

	// TODO?: This is not sorting all the candidates at this count, just the
	// ones that were randomly added at this priority level.
    std::sort(locBuffer, locBuffer+numAdded, doCompare(*this) );

	return numAdded;
}

