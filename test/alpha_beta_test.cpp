#include "gmock/gmock.h"

#define private public

#include "loc.h"
#include "alpha_beta.h"
#include "bdebug.h"

class AlphaBetaFixture : public testing::Test {
public:

};

class MockBridge : public IABBridge
{
public:
	MockBridge() : IABBridge() {}

    MOCK_CONST_METHOD0(isOneMove, bool());
    MOCK_METHOD0(getOnlyMove, Loc());
	MOCK_METHOD0(getUtilityAndMove, std::pair<UtilityValue, Loc>());
    MOCK_METHOD0(makeNextMove, Loc());
    MOCK_METHOD0(undoLastMove, void());
    MOCK_CONST_METHOD0(isCutoff, bool());
};

using ::testing::AtLeast;
using ::testing::Return;
using ::testing::InSequence;

TEST_F(AlphaBetaFixture, NoOptions) {
	// Do a search of a tree with no possible moves
	// - should return an invalid Loc instance
	MockBridge mb;
	
	EXPECT_CALL(mb, isOneMove())
      .WillOnce(Return(true))
      ;
	EXPECT_CALL(mb, getOnlyMove())
      .WillOnce(Return(Loc::INVALID))
      ;

    AlphaBeta ab(mb);

	Loc move = ab.getBestMove();
	EXPECT_EQ(move.isValid(), false);
}

TEST_F(AlphaBetaFixture, FindFromOnlyOneOption) {
	// Do a search of a tree with a single possible move
	// - should find that move, and return it.
	// We shouldn't need to evaluate the utility function for that child
	// node, since there is only one choice.
	MockBridge mb;
	
    Loc locFromBridge(2,3);
	EXPECT_CALL(mb, isOneMove())
      .WillOnce(Return(true))
      ;
	EXPECT_CALL(mb, getOnlyMove())
      .WillOnce(Return(locFromBridge))
      ;

    AlphaBeta ab(mb);

	Loc move = ab.getBestMove();
	EXPECT_TRUE(move.isValid());
	EXPECT_EQ(move, locFromBridge);
}

TEST_F(AlphaBetaFixture, FindFromTwoOptions) {
	// Do a search of a tree with two possible moves, both terminal positions
	// - should find that move, and return it.
	// We need to evaluate the utility function for each terminal child node
	MockBridge mb;
	
    Loc locFromBridge1(2,3);
    std::pair<UtilityValue, Loc> ulpair1(20.5, locFromBridge1);
    Loc locFromBridge2(4,5);
    std::pair<UtilityValue, Loc> ulpair2(48.2, locFromBridge2);

	InSequence dummy;

	EXPECT_CALL(mb, isOneMove())
      .WillOnce(Return(false))
      ;
	EXPECT_CALL(mb, isCutoff())
      .WillOnce(Return(false))
      ;
	EXPECT_CALL(mb, makeNextMove())
      .WillOnce(Return(locFromBridge1))
      ;
	EXPECT_CALL(mb, isCutoff())
      .WillOnce(Return(true))
      ;
	EXPECT_CALL(mb, getUtilityAndMove())
      .WillOnce(Return(ulpair1))
      ;
	EXPECT_CALL(mb, undoLastMove())
      ;
	EXPECT_CALL(mb, makeNextMove())
      .WillOnce(Return(locFromBridge2))
      ;
	EXPECT_CALL(mb, isCutoff())
      .WillOnce(Return(true))
      ;
	EXPECT_CALL(mb, getUtilityAndMove())
      .WillOnce(Return(ulpair2))
      ;
	EXPECT_CALL(mb, undoLastMove())
      ;
	EXPECT_CALL(mb, makeNextMove())
      .WillOnce(Return(Loc::INVALID))
      ;

    AlphaBeta ab(mb);

	BD(cout << "1 before getBestMove" << endl);
	Loc move = ab.getBestMove();
	BD(cout << "x after getBestMove, (" << (int)move[0] << ',' << (int)move[1] << ')' << endl);
	BD(cout << "expected: " << locFromBridge2._value << endl);
	EXPECT_TRUE(move.isValid());
	EXPECT_EQ(locFromBridge2, move);
}

TEST_F(AlphaBetaFixture, FindFromTwoOptionsReversed) {
	// Same as above, only searching the best move first
	// - should find the first move, and return it.
	// We need to evaluate the utility function for each terminal child node
	MockBridge mb;
	
    Loc locFromBridge1(2,3);
    std::pair<UtilityValue, Loc> ulpair1(20.5, locFromBridge1);
    Loc locFromBridge2(4,5);
    std::pair<UtilityValue, Loc> ulpair2(17.2, locFromBridge2);

	InSequence dummy;

	EXPECT_CALL(mb, isOneMove())
      .WillOnce(Return(false))
      ;
	EXPECT_CALL(mb, isCutoff())
      .WillOnce(Return(false))
      ;
	EXPECT_CALL(mb, makeNextMove())
      .WillOnce(Return(locFromBridge1))
      ;
	EXPECT_CALL(mb, isCutoff())
      .WillOnce(Return(true))
      ;
	EXPECT_CALL(mb, getUtilityAndMove())
      .WillOnce(Return(ulpair1))
      ;
	EXPECT_CALL(mb, undoLastMove())
      ;
	EXPECT_CALL(mb, makeNextMove())
      .WillOnce(Return(locFromBridge2))
      ;
	EXPECT_CALL(mb, isCutoff())
      .WillOnce(Return(true))
      ;
	EXPECT_CALL(mb, getUtilityAndMove())
      .WillOnce(Return(ulpair2))
      ;
	EXPECT_CALL(mb, undoLastMove())
      ;
	EXPECT_CALL(mb, makeNextMove())
      .WillOnce(Return(Loc::INVALID))
      ;

    AlphaBeta ab(mb);

	BD(cout << "1 before getBestMove" << endl);
	Loc move = ab.getBestMove();
	BD(cout << "x after getBestMove, (" << (int)move[0] << ',' << (int)move[1] << ')' << endl);
	BD(cout << "expected: " << locFromBridge1._value << endl);
	EXPECT_TRUE(move.isValid());
	EXPECT_EQ(locFromBridge1, move);
}

TEST_F(AlphaBetaFixture, OpponentChoosesBadMoveForUs) {
	// Same as above, only searching the best move first
	// - should find the first move, and return it.
	// We need to evaluate the utility function for each terminal child node
	MockBridge mb;

    Loc loc1(1,1);
    std::pair<UtilityValue, Loc> ulpair1(1.0, loc1);
    Loc loc2(2,2);
    std::pair<UtilityValue, Loc> ulpair2(2.0, loc2);
    Loc loc3(3,3);
    std::pair<UtilityValue, Loc> ulpair3(3.0, loc3);
    Loc loc4(4,4);
    std::pair<UtilityValue, Loc> ulpair4(4.0, loc4);

	InSequence dummy;

	// This code is indented with the depth of the search.
	EXPECT_CALL(mb, isOneMove())
      .WillOnce(Return(false))
      ;
	EXPECT_CALL(mb, isCutoff())
      .WillOnce(Return(false))
      ;
	EXPECT_CALL(mb, makeNextMove())
      .WillOnce(Return(loc1))
      ;
		EXPECT_CALL(mb, isCutoff())
		  .WillOnce(Return(false))
		  ;
		EXPECT_CALL(mb, makeNextMove())
		  .WillOnce(Return(loc3))
		  ;
			EXPECT_CALL(mb, isCutoff())
			  .WillOnce(Return(true))
			  ;
			EXPECT_CALL(mb, getUtilityAndMove())
			  .WillOnce(Return(ulpair3))
			  ;
		EXPECT_CALL(mb, undoLastMove())
		  ;
		EXPECT_CALL(mb, makeNextMove())
		  .WillOnce(Return(loc4))
		  ;
			EXPECT_CALL(mb, isCutoff())
			  .WillOnce(Return(true))
			  ;
			EXPECT_CALL(mb, getUtilityAndMove())
			  .WillOnce(Return(ulpair4))
			  ;
		EXPECT_CALL(mb, undoLastMove())
		  ;
		EXPECT_CALL(mb, makeNextMove())
		  .WillOnce(Return(Loc::INVALID))
		  ;
	EXPECT_CALL(mb, undoLastMove())
      ;
	EXPECT_CALL(mb, makeNextMove())
      .WillOnce(Return(loc2))
      ;
		EXPECT_CALL(mb, isCutoff())
		  .WillOnce(Return(true))
		  ;
		EXPECT_CALL(mb, getUtilityAndMove())
		  .WillOnce(Return(ulpair2))
		  ;
	EXPECT_CALL(mb, undoLastMove())
      ;
	EXPECT_CALL(mb, makeNextMove())
	  .WillOnce(Return(Loc::INVALID))
	  ;

    AlphaBeta ab(mb);

	BD(cout << "1 before getBestMove" << endl);
	Loc move = ab.getBestMove();
	BD(cout << "x after getBestMove, (" << (int)move[0] << ',' << (int)move[1] << ')' << endl);
	EXPECT_TRUE(move.isValid());
	EXPECT_EQ(loc3, move);
}

#if 0
    def test_top_level_options(self):
        game = MockGame([
            MockState("S0", 1, [(0,"S1"),(1,"S2"),(2,"S3")]),
            MockState("S1", 1, []),
            MockState("S2", 2, []),
            MockState("S3", 1, [])])
        action, value = alphabeta_search(state="S0", game=game)
        self.assertEquals(action, (1, "S2"))
        self.assertEquals(value, 2)
#endif

#if 0

#!/usr/bin/env python

import unittest

from pentai.ai.alpha_beta import *

def mock_undo(*args, **kwargs):
    pass

class MockState(object):
    def __init__(self, name, utility, successors):
        self.name = name
        self.utility = utility
        self.successors = successors
        '''
        for s in successors:
           s.undo_last_move = mock_undo
        '''

class MockGame:
    def __init__(self, states, max_depth=4):
        self.states = dict([(s.name, s) for s in states])
        self.max_depth = max_depth
     
    def successors(self, state_name, depth):
        state = self.states[state_name]
        for child_state in state.successors:
            yield child_state

    def utility(self, state_name, depth):
        return self.states[state_name].utility

    def terminal_test(self, state_name, depth):
        if depth >= self.max_depth:
            return True
        return len(self.states[state_name].successors) == 0

    def to_move(self, state_name):
        return True # TODO?

    def save_utility(self, state, depth, utility_value):
        pass

    def report_short_circuit(self, *args):
        pass

    def report_vals(self, *args):
        pass

    def use_bl_cutoff(self):
        return False

class AlphaBetaTest(unittest.TestCase):
    '''
    # TODO: Resurrect
    def test_finished_game(self):
        game = mock.Mock(
            utility=0,
            terminal_test=True,
            to_move=True,
            successors=[("move","child_state")])
        action, value = alphabeta_search(state="start_state", game=game)
        self.assertEquals(action, ("move", "child_state"))
        self.assertEquals(value, 0)
    '''


    def test_top_level_with_one_move_having_a_single_descendent(self):
        game = MockGame([
            MockState("S0", 1, [(0,"S1"),(1,"S2"),(2,"S3")]),
            MockState("S1", 1, []),
            MockState("S2", 2, []),
            MockState("S3", 1, [(0,"S4")]),
            MockState("S4", 4, [])])
        action, value = alphabeta_search(state="S0", game=game)
        self.assertEquals(action, (2, "S3"))
        self.assertEquals(value, 4)

    def test_only_search_one_depth_level(self):
        game = MockGame([
            MockState("S0", 0, [(0,"S1"),(0,"S1")]),
            MockState("S1", 1, [(0,"S2")]),
            MockState("S2", 2, [(0,"S3")]),
            MockState("S3", 3, [])], max_depth=1)
        action, value = alphabeta_search(state="S0", game=game)
        self.assertEquals(value, 1)

    def test_only_search_two_depth_levels(self):
        game = MockGame([
            MockState("S0", 0, [(0,"S1"),(0,"S1")]),
            MockState("S1", 1, [(0,"S2")]),
            MockState("S2", 2, [(0,"S3")]),
            MockState("S3", 3, [])], max_depth=2)
        action, value = alphabeta_search(state="S0", game=game)
        self.assertEquals(value, 2)

    # !python pentai/ai/t_alpha_beta.py AlphaBetaTest.test_terminal_state
    def test_terminal_state(self):
        game = MockGame([
            MockState("S0", 0, [(0,"S1"),(0,"S1")]),
            MockState("S1", 1, [(0,"S2")]),
            MockState("S2", 2, [(0,"S3")]),
            MockState("S3", 3, [])], max_depth=4)
        action, value = alphabeta_search(state="S0", game=game)
        self.assertEquals(value, 3.0)

if __name__ == "__main__":
    unittest.main()

#endif


