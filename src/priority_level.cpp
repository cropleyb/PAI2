#include "priority_level.h"


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

int
PriorityLevel::copyCands(Loc *candidateBuffer)
{
	return 0;
}

#if 0
    def add_or_remove_candidates(self, cand_list, inc):
        for cand_loc in cand_list:
            self.add_or_remove_candidate(cand_loc, inc)
#endif

void
PriorityLevel::addOrRemoveCandidate(Loc candLoc, int inc)
{
	// Get the index of the node for the given candidate location
	CompressedLoc candLocVal = candLoc._value;
	Ind nodeInd = _nodeIndByLoc[candLocVal];

	if (nodeInd < 0)
	{
		if (inc < 0)
		{
			return;
		}
		// assert(inc > 0)

		// Node for this loc has no count yet
		// Use the next node from the free list
		nodeInd = _freeListInd;
		_nodeIndByLoc[candLocVal] = nodeInd;
		DLNode node = _dlNodes[nodeInd];
		_freeListInd = node._nextInd;

		Ind nextInd = _dlHeadInd;
		Ind oldHeadInd = _dlHeadInd;
		node.setPrevInd(-1); // "node" is currently the head of the list
		node.setNextInd(nextInd);

		if (oldHeadInd >= 0)
		{
			DLNode &oldHead = _dlNodes[oldHeadInd];
			oldHead.setPrevInd(nodeInd);
		}

		_dlHeadInd = nodeInd;
		node.setLoc(candLoc);
		node.adjustCount(inc);

		_numCands += 1;
	}
	else
	{
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

#if 0

class PriorityLevel:
    MAXnODES = 19*19

    def _init__(self):
        self.free_list_ind = 0
        self.dl_head_ind = -1
        self.dl_nodes = []
        self.num_cands = 0
        for i in range(self.MAX_NODES):
            node = DLNode()
            node.set_next_ind(i+1)
            self.dl_nodes.append(node)

        node.set_next_ind(-1)

        self.node_ind_by_loc = [-1] * self.MAX_NODES

    def add_or_remove_candidates(self, cand_list, inc):
        for cand_loc in cand_list:
            self.add_or_remove_candidate(cand_loc, inc)

    def add_or_remove_candidate(self, cand_loc, inc):
        assert(cand_loc.__class__ == Loc)
        # Get the index of the node for the given candidate location
        cand_loc_val = cand_loc.val
        node_ind = self.node_ind_by_loc[cand_loc_val]

        dl_nodes = self.dl_nodes

        if node_ind < 0:
            if inc < 0:
                return
            #assert(inc > 0)

            # Node for this loc has no count yet
            # Use the next node from the free list
            node_ind = self.free_list_ind
            self.node_ind_by_loc[cand_loc_val] = node_ind
            node = dl_nodes[node_ind]
            self.free_list_ind = node.next_ind

            next_ind = old_head_ind = self.dl_head_ind
            node.set_prev_ind(-1) # "node" is currently the head of the list
            node.set_next_ind(next_ind)

            if old_head_ind >= 0:
                old_head = dl_nodes[old_head_ind]
                old_head.set_prev_ind(node_ind)

            self.dl_head_ind = node_ind
            node.set_loc(cand_loc)
            node.adjust_count(inc)

            self.num_cands += 1

        else:
            # A node for this loc has already been used, update its count
            node = dl_nodes[node_ind]
            new_count = node.adjust_count(inc)

            if new_count <= 0:
                # We don't need this node any more
                assert(new_count == 0)

                # Remove it from its current place in the dl_list
                prev_node_ind = node.prev_ind
                next_node_ind = node.next_ind

                if prev_node_ind >= 0:
                    prev_node = dl_nodes[prev_node_ind]
                    prev_node.set_next_ind(next_node_ind)

                if next_node_ind >= 0:
                    next_node = dl_nodes[next_node_ind]
                    next_node.set_prev_ind(prev_node_ind)

                if self.dl_head_ind == node_ind:
                    #st()
                    new_head_ind = node.next_ind
                    self.dl_head_ind = new_head_ind

                self.node_ind_by_loc[cand_loc_val] = -1

                # TODO Put it at the head of the free list
                node.next_ind = self.free_list_ind
                self.free_list_ind = node_ind

                self.num_cands -= 1

    def get_num_cands(self):
        return self.num_cands

    #def __iter__(self):
    def get_iter(self):
        dl_nodes = self.dl_nodes
        curr_ind = self.dl_head_ind

        while curr_ind >= 0:
            try:
                curr_node = dl_nodes[curr_ind]
            except:
                st()
            curr_loc = curr_node.loc
            curr_ind = curr_node.next_ind
            curr_count = curr_node.count
            #assert curr_ind >= 0
            assert curr_count >= 0
            assert curr_loc.val >= 0
            yield curr_loc, curr_count
#endif
