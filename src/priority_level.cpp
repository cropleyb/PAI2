#include "priority_level.h"

#include "bdebug.h"

#if 0
	// TODO
    _repr__(self):
        return "prev_ind: %s, next_ind: %s, loc: %s, count: %s" % \
            (self.prev_ind, self.next_ind, self.loc.val, self.count)
#endif

PriorityLevel::PriorityLevel()
{
    _freeListInd = 0;
    _dlHeadInd = -1;
    _numCands = 0;

	for (int i=0; i<MAX_NODES; i++)
	{
        _dlNodes[i].setNextInd(i+1);
		_nodeIndByLoc[i] = -1;
	}

	_dlNodes[MAX_NODES-1].setNextInd(-1);
}

#if 0
    def add_or_remove_candidates(self, cand_list, inc):
        for cand_loc in cand_list:
            self.add_or_remove_candidate(cand_loc, inc)
#endif

void
PriorityLevel::addOrRemoveCandidate(Loc candLoc, int inc)
{
	BD(cout << "ARC: " << (void *)this << "----------------" << endl);

	// Get the index of the node for the given candidate location
	CompressedLoc candLocVal = candLoc._value;
	Ind nodeInd = _nodeIndByLoc[candLocVal];

	if (nodeInd < 0)
	{
		BD(cout << "ARC 1" << endl);
		if (inc < 0)
		{
			BD(cout << "ARC 2" << endl);
			// Trying to reduce the count below zero. Error?
			return;
		}
		BD(cout << "ARC 3" << endl);
		// assert(inc > 0)

		// Node for this loc has no count yet
		// Use the next node from the free list
		BD(cout << "ARC 4 - using node " << _freeListInd << endl);
		nodeInd = _freeListInd;
		_nodeIndByLoc[candLocVal] = nodeInd;
		DLNode &node = _dlNodes[nodeInd];
		_freeListInd = node._nextInd;

		Ind nextInd = _dlHeadInd;
		Ind oldHeadInd = _dlHeadInd;
		node.setPrevInd(-1); // "node" is currently the head of the list
		node.setNextInd(nextInd);
		BD(cout << "ARC 4a" << endl);

		if (oldHeadInd >= 0)
		{
			BD(cout << "ARC 5" << endl);
			DLNode &oldHead = _dlNodes[oldHeadInd];
			oldHead.setPrevInd(nodeInd);
		}

		BD(cout << "ARC set head ind to ---------------" << nodeInd << endl);
		_dlHeadInd = nodeInd;
		BD(cout << "ARC set node loc to ---------------" << candLoc._value << endl);
		node.setLoc(candLoc);
		BD(cout << "ARC set inc to " << inc << endl);
		node.adjustCount(inc);

		_numCands += 1;
	}
	else
	{
		BD(cout << "ARC B 1" << endl);
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

Ind
PriorityLevel::getNumCands()
{
	return _numCands;
}

Ind
PriorityLevel::getCands(Loc *locBuffer, Ind *countBuffer, Ind max)
{
	BD(cout << "getCands top - " << (void *)this << endl);
	BD(cout << "getCands 1 - _dlHeadInd" << _dlHeadInd << endl);
	Ind numAdded = 0;
	Ind currInd = _dlHeadInd;

	while ((currInd >= 0) && (numAdded < max))
	{
		BD(cout << "getCands 2 - currInd: " << currInd << endl);
		DLNode &currNode = _dlNodes[currInd];
		if (currNode._loc == Loc::INVALID)
		{
			BD(cout << "getCands 3 - INVALID" << endl);
			break;
		}
		BD(cout << "getCands 4 - adding " << currNode._loc._value << endl);
		locBuffer[numAdded] = currNode._loc;
		countBuffer[numAdded] = currNode._count;
		currInd = currNode._nextInd;
		numAdded++;
	}
	BD(cout << "getCands 5" << endl);

	return numAdded;
}

