#define private public

#include "priority_level.h"
#include "gmock/gmock.h"

#define MAX_CANDS 20

class PriorityLevelFixture : public testing::Test {
public:

};

TEST_F(PriorityLevelFixture, testEmpty)
{
	PriorityLevel pl;
	
	Loc candidateBuffer[MAX_CANDS];

	int candCount = pl.copyCands(candidateBuffer);

	EXPECT_EQ(0, candCount);
}

#if 0
class PriorityLevelTest(unittest.TestCase):
    def setUp(self):
        self.pl = PriorityLevel()

    def arc(self, candidate, inc=1):
        self.pl.add_or_remove_candidate(Loc(candidate), inc)

    def get_iter(self):
        return list(self.pl.get_iter())

    def num_cands(self):
        return self.pl.get_num_cands()
#endif



#if 0
    def test_add_1(self):
        self.arc((4,5))
        l = self.get_iter()
        self.assertEquals(len(l), 1)
        self.assertEquals(self.num_cands(), 1)

    def test_add_2(self):
        self.arc((4,5))
        self.arc((6,7))
        l = self.get_iter()
        self.assertEquals(len(l), 2)
        self.assertEquals(self.num_cands(), 2)

    def test_add_1_twice(self):
        self.arc((4,5))
        self.arc((4,5))
        l = self.get_iter()
        self.assertEquals(len(l), 1)
        self.assertEquals(self.num_cands(), 1)

    def test_add_1_and_remove_it(self):
        self.arc((4,5))
        self.arc((4,5), -1)
        l = self.get_iter()
        self.assertEquals(len(l), 0)
        self.assertEquals(self.num_cands(), 0)

    def test_add_2_remove_1(self):
        self.arc((4,6), inc=1)
        self.arc((5,7), inc=1)
        self.arc((4,6), inc=-1)
        l = list(self.get_iter())

        self.assertEquals(len(l), 1)
        self.assertEquals(l[0],((5,7),1))
#endif

#if 0
from pentai.base.defines import *
from pentai.base.loc import *

class DLNode:
    def __init__(self):
        self.prev_ind = 0
        self.next_ind = 0
        self.loc = -1
        self.count = 0

    def set_prev_ind(self, ind):
        self.prev_ind = ind

    def set_next_ind(self, ind):
        self.next_ind = ind

    def set_loc(self, loc):
        self.loc = loc

    def adjust_count(self, ind):
        self.count += ind
        return self.count

    def __repr__(self):
        return "prev_ind: %s, next_ind: %s, loc: %s, count: %s" % \
            (self.prev_ind, self.next_ind, self.loc.val, self.count)

class PriorityLevel:
    MAX_NODES = 19*19

    def __init__(self):
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


