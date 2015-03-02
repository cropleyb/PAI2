#include "gmock/gmock.h"

#define private public

#include "loc.h"
#include "move_suggester.h"
#include "position_stats.h"
#include "candidate_cache.h"
#include "bdebug.h"

class MoveSuggesterFixture : public testing::Test {
public:
	PositionStats ps;
	CandidateCache cc;
	MoveSuggester ms;

	MoveSuggesterFixture() : ms(ps, cc) {}
};

using ::testing::InSequence;

TEST_F(MoveSuggesterFixture, NoMoves) {
	Loc move = ms.getNextMove(0);

	EXPECT_EQ(move.isValid(), false);
}

using std::vector;
typedef vector<Loc> LocArr;

TEST_F(MoveSuggesterFixture, OneMove) {
	LocArr ll;
	Loc l1(1,1);
	ll.push_back(l1);
	ps.reportLengthCandidates(P2, 2, ll, 1); // length 2, inc

	Loc move = ms.getNextMove(3); // depth
	EXPECT_EQ(move.isValid(), true);
	EXPECT_EQ(l1, move);

	move = ms.getNextMove(3);
	EXPECT_EQ(move.isValid(), false);
}

TEST_F(MoveSuggesterFixture, TwoMovesSamePL) {
	LocArr ll;
	Loc l1(1,1);
	Loc l2(2,2);
	ll.push_back(l2);
	ll.push_back(l1);
	ll.push_back(l1);
	ps.reportLengthCandidates(P1, 4, ll, 1); // length 4, inc

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
	LocArr ll1;
	ll1.push_back(l1);
	ps.reportLengthCandidates(P1, 4, ll1, 1); // length 4, inc

	Loc l2(2,2);
	LocArr ll2;
	ll2.push_back(l2);
	ps.reportLengthCandidates(P1, 2, ll2, 1); // length 2, inc

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
	LocArr ll1;
	ll1.push_back(l1);
	ps.reportLengthCandidates(P1, 4, ll1, 1); // length 4, inc
	ps.reportLengthCandidates(P1, 4, ll1, -1); // length 4, dec

	Loc move = ms.getNextMove(2);
	EXPECT_EQ(move.isValid(), false);
}

TEST_F(MoveSuggesterFixture, Extend3BeforeExtend2) {
	Loc l1(1,1);
	LocArr ll1;
	ll1.push_back(l1);
	ps.reportLengthCandidates(P1, 3, ll1, 1);

	Loc l2(2,2);
	LocArr ll2;
	ll2.push_back(l2);
	ps.reportLengthCandidates(P1, 2, ll2, 1);

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
	LocArr ll1;
	ll1.push_back(l1);
	ps.reportLengthCandidates(P1, 3, ll1, 1);

	Loc l2(2,2);
	LocArr ll2;
	ll2.push_back(l2);
	ps.reportLengthCandidates(P2, 3, ll2, 1);

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
	LocArr ll1;
	ll1.push_back(l1);
	ps.reportLengthCandidates(P1, 3, ll1, 1);

	Loc l2(2,2);
	LocArr ll2;
	ll2.push_back(l2);
	ps.reportLengthCandidates(P2, 3, ll2, 1);

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
	LocArr ll1;
	ll1.push_back(l1);
	ps.reportLengthCandidates(P1, 3, ll1, 1);

	Loc l2(2,2);
	LocArr ll2;
	ll2.push_back(l2);
	ps.reportLengthCandidates(P2, 3, ll2, 1);

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
	LocArr ll1;
	ll1.push_back(l1);
	ps.reportLengthCandidates(P1, 1, ll1, 1);

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

class PriorityFilter6Test(unittest.TestCase):
    def setUp(self):
        self.pf = PriorityFilter6()

    def arc(self, colour, length, candidate_list, inc=1):
        self.pf.add_or_remove_candidates(colour, length, candidate_list, inc)

    def set_captured_by(self, colour, captured):
        self.pf.captured[colour] = captured

    def ar_take(self, *args, **kwargs):
        self.pf.add_or_remove_take(*args, **kwargs)

    def ar_threat(self, *args, **kwargs):
        self.pf.add_or_remove_threat(*args, **kwargs)

    def test_dont_start_in_the_middle_13(self):
    def test_iterate_over_our_four(self):
    def test_iterate_over_one_of_their_fours(self):
    def test_two_of_their_fours_try_the_take(self):
    def test_two_of_their_fours_no_take(self):
    def test_finish_capture_win(self):
    def test_block_or_take_to_defend_capture_loss(self):
    def test_iterate_over_own_black_first(self):
    def test_iterate_over_higher_priority_only(self):
    def test_iterate_over_capture(self):
    def test_iterate_over_own_capture_first(self):
    def test_iterate_over_other_players_four_before_our_capture(self):
    def atest_iterate_over_other_players_capture_before_our_threes(self):
    def test_iterate_block_only(self):
    def test_iterate_over_capture(self):
    def test_iterate_over_their_capture_before_our_two(self):
    def test_iterate_over_their_three_before_our_threat(self):
    def test_add_and_remove_length_candidate(self):
    def test_add_and_remove_capture_candidate(self):
    def test_add_and_remove_threat_candidate(self):
    def test_add_and_remove_length_candidate_from_diff_directions(self):
    def test_multiple_entries_searched_first(self):
    def test_choose_the_only_winner(self):
    def atest_one_opponent_double_three_must_be_block_cap_or_threatened(self):
    def atest_three_plus_opponent_double_threes_cannot_block(self):
#endif

