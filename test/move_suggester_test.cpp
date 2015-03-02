#include "gmock/gmock.h"

#define private public

#include "loc.h"
#include "move_suggester.h"
#include "position_stats.h"
#include "candidate_cache.h"
#include "bdebug.h"

using std::vector;
typedef vector<Loc> LocArr;

class MoveSuggesterFixture : public testing::Test {
public:
	PositionStats ps;
	CandidateCache cc;
	MoveSuggester ms;

	MoveSuggesterFixture() : ms(ps, cc) {}

	void addLocs(LocArr & /* ll */)
	{
	}

	template<typename ... Types>
	void addLocs(LocArr &ll, Loc first, Types ... rest)
	{
		ll.push_back(first);
		addLocs(ll, rest...);
	}

	template<typename ... Types>
	void arcs(Colour c, Length l, int inc, Types ... rest)
	{
		LocArr ll;
		addLocs(ll, rest...);
		ps.reportLengthCandidates(c, l, ll, inc);
	}

#if 0
    void set_captured_by(self, colour, captured)
	{
        pf.captured[colour] = captured;
	}

    void ar_take(self, *args, **kwargs)
	{
        pf.add_or_remove_take(*args, **kwargs);
	}

    void ar_threat(self, *args, **kwargs)
	{
        pf.add_or_remove_threat(*args, **kwargs);
	}
#endif
};

using ::testing::InSequence;

TEST_F(MoveSuggesterFixture, NoMoves) {
	Loc move = ms.getNextMove(0);

	EXPECT_EQ(move.isValid(), false);
}

TEST_F(MoveSuggesterFixture, OneMove) {
	Loc l1(1,1);
	arcs(P2, 2, 1, l1); // length 2, inc

	Loc move = ms.getNextMove(3); // depth
	EXPECT_EQ(move.isValid(), true);
	EXPECT_EQ(l1, move);

	move = ms.getNextMove(3);
	EXPECT_EQ(move.isValid(), false);
}

TEST_F(MoveSuggesterFixture, TwoMovesSamePL) {
	Loc l1(1,1);
	Loc l2(2,2);
	arcs(P1, 4, 1, l2, l1, l1); // length 2, inc

	Loc move = ms.getNextMove(2);
	EXPECT_EQ(move.isValid(), true);
	EXPECT_EQ(l1, move);

	move = ms.getNextMove(2);
	EXPECT_EQ(move.isValid(), true);
	EXPECT_EQ(l2, move);

	move = ms.getNextMove(2);
	EXPECT_EQ(move.isValid(), false);
}

TEST_F(MoveSuggesterFixture, TwoMovesDiffPL) {
	Loc l1(1,1);
	arcs(P1, 4, 1, l1); // length 2, inc

	Loc l2(1,1);
	arcs(P1, 2, 1, l2); // length 2, inc

	Loc move = ms.getNextMove(2);
	EXPECT_EQ(move.isValid(), true);
	EXPECT_EQ(l1, move);

	move = ms.getNextMove(2);
	EXPECT_EQ(move.isValid(), true);
	EXPECT_EQ(l2, move);

	move = ms.getNextMove(2);
	EXPECT_EQ(move.isValid(), false);
}

TEST_F(MoveSuggesterFixture, AddAndThenRemoveSameLoc) {
	Loc l1(1,1);
	arcs(P1, 4, 1, l1); // length 4, inc
	arcs(P1, 4, -1, l1); // length 4, dec

	Loc move = ms.getNextMove(2);
	EXPECT_EQ(move.isValid(), false);
}

TEST_F(MoveSuggesterFixture, Extend3BeforeExtend2) {
	Loc l1(1,1);
	arcs(P1, 3, 1, l1);

	Loc l2(2,2);
	arcs(P1, 2, 1, l2);

	Loc move = ms.getNextMove(2);
	EXPECT_EQ(move.isValid(), true);
	EXPECT_EQ(l1, move);

	move = ms.getNextMove(2);
	EXPECT_EQ(move.isValid(), true);
	EXPECT_EQ(l2, move);

	move = ms.getNextMove(2);
	EXPECT_EQ(move.isValid(), false);
}

TEST_F(MoveSuggesterFixture, ExtendOursBeforeTheirs) {
	Loc l1(1,1);
	arcs(P1, 3, 1, l1);

	Loc l2(2,2);
	arcs(P2, 3, 1, l2);

	Loc move = ms.getNextMove(0);
	EXPECT_EQ(move.isValid(), true);
	EXPECT_EQ(l1, move);

	move = ms.getNextMove(0);
	EXPECT_EQ(move.isValid(), true);
	EXPECT_EQ(l2, move);

	move = ms.getNextMove(0);
	EXPECT_EQ(move.isValid(), false);
}

TEST_F(MoveSuggesterFixture, ExtendTheirsBeforeOurs) {
	Loc l1(1,1);
	arcs(P1, 3, 1, l1);

	Loc l2(2,2);
	arcs(P2, 3, 1, l2);

	Loc move = ms.getNextMove(1);
	EXPECT_EQ(move.isValid(), true);
	EXPECT_EQ(l2, move);

	move = ms.getNextMove(1);
	EXPECT_EQ(move.isValid(), true);
	EXPECT_EQ(l1, move);

	move = ms.getNextMove(1);
	EXPECT_EQ(move.isValid(), false);
}

TEST_F(MoveSuggesterFixture, IterateTwiceNoChange) {
	Loc l1(1,1);
	arcs(P1, 3, 1, l1);

	Loc l2(2,2);
	arcs(P2, 3, 1, l2);

	Loc move = ms.getNextMove(2);
	EXPECT_EQ(l1, move);

	move = ms.getNextMove(2);
	EXPECT_EQ(l2, move);

	move = ms.getNextMove(2);
	EXPECT_EQ(move.isValid(), false);

	move = ms.getNextMove(2);
	EXPECT_EQ(l1, move);

	move = ms.getNextMove(2);
	EXPECT_EQ(l2, move);

	move = ms.getNextMove(2);
	EXPECT_EQ(move.isValid(), false);
}

TEST_F(MoveSuggesterFixture, IterateDifferentDepths) {
	Loc l1(1,1);
	arcs(P1, 1, 1, l1);

	Loc move = ms.getNextMove(0);
	EXPECT_EQ(move.isValid(), true);
	EXPECT_EQ(l1, move);

	move = ms.getNextMove(1);
	EXPECT_EQ(move.isValid(), true);
	EXPECT_EQ(l1, move);

	move = ms.getNextMove(2);
	EXPECT_EQ(move.isValid(), true);
	EXPECT_EQ(l1, move);
}

#if 0

#!/usr/bin/env python

import unittest

from pentai.base.board import *
from pentai.ai.priority_filter_6 import *

class PriorityFilter6Test(unittest.TestCase)
{
    void setUp(self)
	{
        pf = PriorityFilter6();
	}

    void arc(self, colour, length, candidate_list, inc=1)
	{
        pf.add_or_remove_candidates(colour, length, candidate_list, inc);
	}

    void set_captured_by(self, colour, captured)
	{
        pf.captured[colour] = captured;
	}

    void ar_take(self, *args, **kwargs)
	{
        pf.add_or_remove_take(*args, **kwargs);
	}

    void ar_threat(self, *args, **kwargs)
	{
        pf.add_or_remove_threat(*args, **kwargs);
	}

    void test_dont_start_in_the_middle_13(self)
	{
        l = list(pf.get_iter(P1));
        assertEquals(len(l), 0);
	}

    void test_add_and_remove(self)
	{
        arc(P1, 4, ((3,4),));
        arc(P1, 4, ((3,4),), -1);
        arc(P1, 3, ((3,2),));
        l = list(pf.get_iter(P1));
        assertEquals(len(l), 1);
        assertEquals(l[0],(3,2));
	}

    void test_iterate_over_our_four(self)
	{
        arc(P1, 4, ((3,4),));
        arc(P1, 3, ((3,2),));
        l = list(pf.get_iter(P1));
        assertEquals(len(l), 1);
        assertEquals(l[0],(3,4));
	}

    void test_iterate_over_one_of_their_fours(self)
	{
        arc(P2, 4, ((3,4),));
        ar_take(P1, (3,2));
        set_captured_by(P1, 6);
        l = list(pf.get_iter(P1));
        assertEquals(len(l), 2);
        assertEquals(l[0],(3,4));
        assertEquals(l[1],(3,2));
	}

    void test_two_of_their_fours_try_the_take(self)
	{
        arc(P2, 4, ((1,2),));
        arc(P2, 4, ((3,4),));
        ar_take(P1, (3,2));
        l = list(pf.get_iter(P1));
        assertEquals(len(l), 1);
        assertEquals(l[0],(3,2));
	}

    void test_two_of_their_fours_no_take(self)
	{
        #st();
        arc(P2, 4, ((1,2),));
        arc(P2, 4, ((3,4),));
        l = list(pf.get_iter(P1));
        assertEquals(len(l), 1);
        # It doesn't matter which one we choose, we're lost
        # Evaluating this node should give the result
        # But we need to choose one or the other
	}

    void test_finish_capture_win(self)
	{
        set_captured_by(P1, 8);
        ar_take(P1, (1,2));
        arc(P2, 4, ((3,4),));
        l = list(pf.get_iter(P1));
        assertEquals(len(l), 1);
        assertEquals(l[0],(1,2));
	}

    void test_block_or_take_to_defend_capture_loss(self)
	{
        set_captured_by(P2, 8);
        ar_take(P1, (1,2));
        ar_take(P2, (3,4));
        l = list(pf.get_iter(P1));
        assertEquals(len(l), 2);
	}

    void test_iterate_over_own_black_first(self)
	{
        arc(P2, 4, ((1,5),));
        arc(P1, 4, ((3,4),));
        l = list(pf.get_iter(P1));
        assertEquals(len(l), 1);
        assertEquals(l[0],(3,4));
	}

    void test_iterate_over_higher_priority_only(self)
	{
        arc(P2, 3, ((1,5),));
        arc(P2, 4, ((3,4),));
        l = list(pf.get_iter(P1));
        assertEquals(len(l), 1);
        assertEquals(l[0],(3,4));
	}

    void test_iterate_over_capture(self)
	{
        pf.add_or_remove_take(P1, (3,4));
        l = list(pf.get_iter(P1));
        assertEquals(len(l), 1);
        assertEquals(l[0],(3,4));
	}

    void test_iterate_over_own_capture_first(self)
	{
        pf.add_or_remove_take(P1, (1,2));
        pf.add_or_remove_take(P2, (3,4));
        l = list(pf.get_iter(P2));
        assertEquals(len(l), 2);
        assertEquals(l[0],(3,4));
        assertEquals(l[1],(1,2));
	}

    void test_iterate_over_other_players_four_before_our_capture(self)
	{
        pf.add_or_remove_take(P2, (7,2));
        arc(P1, 4, ((3,4),));
        l = list(pf.get_iter(P2));
        assertEquals(len(l), 2);
        assertEquals(l[0],(3,4));
        assertEquals(l[1],(7,2));
	}

    void atest_iterate_over_other_players_capture_before_our_threes(self)
	{
        arc(P1, 3, ((3,4),(1,5)));
        pf.add_or_remove_take(P2, (7,2));
        l = list(pf.get_iter(P2));
        assertEquals(len(l), 3);
        assertEquals(l[0],(7,2));
        our_threes = ((3,4),(1,5));
        assertIn(l[1], our_threes);
        assertIn(l[2], our_threes);
	}

    void test_iterate_block_only(self)
	{
        arc(P2, 3, ((1,5),(2,4)));
        pf.add_or_remove_take(P1, (1,5));
        arc(P1, 4, ((2,4),));
        l = list(pf.get_iter(P2));
        assertEquals(len(l), 1);
        assertEquals(l[0],(2,4));
	}

    void test_iterate_over_capture(self)
	{
        pf.add_or_remove_take(P1, (1,5));
        l = list(pf.get_iter(P2));
        assertEquals(len(l), 1);
        assertEquals(l[0],(1,5));
	}

    void test_iterate_over_their_capture_before_our_two(self)
	{
        arc(P1, 2, ((2,4),(4,6),(5,7)));
        pf.add_or_remove_take(P2, (1,5));
        l = list(pf.get_iter(P1));
        assertEquals(len(l), 4);
        assertEquals(l[0],(1,5));
        twos = (2,4),(4,6),(5,7);
        assertIn(l[1], twos);
        assertIn(l[2], twos);
        assertIn(l[3], twos);
	}

    void test_iterate_over_their_three_before_our_threat(self)
	{
        arc(P1, 3, ((2,4),(4,6),));
        pf.add_or_remove_threat(P2, (1,5));
        l = list(pf.get_iter(P1));
        assertEquals(len(l), 3);
        threes = (2,4),(4,6);
        assertIn(l[0], threes);
        assertIn(l[1], threes);
        assertEquals(l[2],(1,5));
        
    void test_add_and_remove_length_candidate(self)
	{
        #st();
        arc(P1, 3, ((2,4),(4,6),), inc=1);
        pf.add_or_remove_threat(P1, (1,5));
        arc(P1, 3, ((2,4),(4,6),), inc=-1);
        l = list(pf.get_iter(P1));
        assertEquals(len(l), 1);
        assertEquals(l[0],(1,5));
	}

    void test_add_and_remove_capture_candidate(self)
	{
        pf.add_or_remove_take(P1, (1,5), inc=1);
        pf.add_or_remove_take(P1, (1,5), inc=-1);
        l = list(pf.get_iter(P1));
        assertEquals(len(l), 0);
	}

    void test_add_and_remove_threat_candidate(self)
	{
        pf.add_or_remove_threat(P1, (1,5), inc=1);
        pf.add_or_remove_threat(P1, (1,5), inc=-1);
        l = list(pf.get_iter(P1));
        assertEquals(len(l), 0);
	}

    void test_add_and_remove_length_candidate_from_diff_directions(self)
	{
        arc(P1, 3, ((2,4),(4,6),), inc=1);
        arc(P1, 3, ((2,4),(3,3),), inc=1);
        arc(P1, 3, ((2,4),(4,6),), inc=-1);
        l = list(pf.get_iter(P1));
        assertEquals(len(l), 2);
        pair = ((2,4),(3,3),);
        assertIn(l[0], pair);
        assertIn(l[1], pair);
	}

    void test_multiple_entries_searched_first(self)
	{
        arc(P1, 3, ((2,4),(4,6),), inc=1);
        arc(P1, 3, ((2,4),(3,3),), inc=1);
        l = list(pf.get_iter(P1));
        assertEquals(len(l), 3);
        assertEquals(l[0],(2,4));
        others = ((4,6), (3,3));
        assertIn(l[1], others);
        assertIn(l[2], others);
	}

    void test_choose_the_only_winner(self)
	{
        arc(P1, 4, ((4,6),), inc=1);
        arc(P1, 4, ((5,7),), inc=1);
        arc(P1, 4, ((4,6),), inc=-1);
        l = list(pf.get_iter(P1));
        assertEquals(len(l), 1);
        assertEquals(l[0],(5,7));
	}

    void atest_one_opponent_double_three_must_be_block_cap_or_threatened(self)
	{
        # i.e. a single instance of a double 3 attack must be blocked,
        # captured, or threatened, or we must extend a 3 of our own
        pf.add_or_remove_take(P2, (1,5), inc=1);

        # Only their threes needs to be looked at, since there is a double 3
        arc(P1, 3, ((4,6),(5,6),), inc=1);
        arc(P1, 3, ((5,6),(9,6),), inc=1);

        # all our 3s should be included
        arc(P2, 3, ((4,8),(10,6),), inc=1);

        # only threats that threaten the double 3 really need considering (TODO)
        pf.add_or_remove_threat(P2, (2,9), inc=1);

        # All 2s are irrelevant since there is a double 3
        arc(P2, 2, ((7,8),(8,8),(10,8)), inc=1);
        arc(P2, 2, ((8,8),(10,8),(12,8)), inc=1);
        arc(P2, 2, ((10,8),(12,8),(13,8)), inc=1);

        #st()
        l = list(pf.get_iter(P2));

        assertEquals(len(l), 5);
        assertEquals(l[0], (5,6)); # Their open 3
        assertEquals(l[1], (1,5)); # Our take
        our_threes = ((4,8), (10,6)); # Our 3s
        assertIn(l[2], our_threes);
        assertIn(l[3], our_threes);
        assertEquals(l[4], (2,9)); # Our threat (TODO)
	}

    void atest_three_plus_opponent_double_threes_cannot_block(self)
	{
        # i.e. a single instance of a double 3 attack must be blocked,
        # captured, or threatened, or we must extend a 3 of our own
        pf.add_or_remove_take(P2, (1,5), inc=1)

        # 3 x Two double attacks - can't block
        arc(P1, 3, ((4,6),(5,6),), inc=1);
        arc(P1, 3, ((5,6),(9,6),), inc=1);
        arc(P1, 3, ((4,6),(9,6),), inc=1);

        # all our 3s should be included
        arc(P2, 3, ((4,8),(10,6),), inc=1);

        # only threats that threaten the double 3 really need considering (TODO)
        pf.add_or_remove_threat(P2, (2,9), inc=1);

        # All 2s are irrelevant since there is a double 3
        arc(P2, 2, ((7,8),(8,8),(10,8)), inc=1);
        arc(P2, 2, ((8,8),(10,8),(12,8)), inc=1);
        arc(P2, 2, ((10,8),(12,8),(13,8)), inc=1);

        l = list(pf.get_iter(P2));
        assertEquals(len(l), 4);
        assertEquals(l[0], (1,5)); # Our take
        our_threes = ((4,8), (10,6)); # Our 3s
        assertIn(l[1], our_threes);
        assertIn(l[2], our_threes);
        assertEquals(l[3], (2,9)); # Our threat (TODO)
	}

#endif

