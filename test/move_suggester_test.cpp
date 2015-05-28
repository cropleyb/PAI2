#include "gmock/gmock.h"

#define private public

#include "loc.h"
#include "move_suggester.h"
#include "position_stats.h"
#include "candidate_cache.h"
#include "bdebug.h"

using std::vector;
typedef vector<Loc> LocArr;
typedef unsigned long UL;

using std::set;
class LocSet : public set<Loc>
{
public:
	LocSet(Loc l) { insert(l); }
	LocSet(Loc l1, Loc l2) { insert(l1); insert(l2); }
	LocSet(Loc l1, Loc l2, Loc l3) { insert(l1); insert(l2); insert(l3); }

	bool contains(const Loc &l) const { return count(l) > 0; }
};

class MoveSuggesterFixture : public testing::Test {
public:
	PositionStats ps;
	MoveSuggester ms;

	MoveSuggesterFixture() : ms(ps) {}

	LocArr getLocsInOrder(Depth d)
	{
		LocArr la;
		for (Loc loc=ms.getNextMove(d, P1); loc.isValid(); loc=ms.getNextMove(d, P1)) {
			la.push_back(loc);
		}
		return la;
	}

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
	void arcs(Colour c, int l, int inc, Types ... rest)
	{
		LocArr ll;
		addLocs(ll, rest...);
		ps.reportCandidates(c, (LinePatternType)l, ll, inc);
	}

	// Hack for testing...
    void setCapturedBy(Colour colour, int count)
	{
		ps._captured[colour] = count;
	}
};

using ::testing::InSequence;

TEST_F(MoveSuggesterFixture, NoMoves) {
	Loc move = ms.getNextMove(0, P1);

	EXPECT_EQ(move.isValid(), false);
}

TEST_F(MoveSuggesterFixture, OneMove) {
	Loc l1(1,1);
	arcs(P2, Line2, 1, l1); // length 2, inc

	Loc move = ms.getNextMove(3, P2); // depth, search player
	EXPECT_EQ(move.isValid(), true);
	EXPECT_EQ(l1, move);

	move = ms.getNextMove(3, P2);
	EXPECT_EQ(move.isValid(), false);
}

TEST_F(MoveSuggesterFixture, TwoMovesSamePL) {
	Loc l1(1,1);
	Loc l2(2,2);
	arcs(P1, Line3, 1, l2, l1, l1); // length 2, inc

	Loc move = ms.getNextMove(2, P1);
	EXPECT_EQ(move.isValid(), true);
	EXPECT_EQ(l1, move);

	move = ms.getNextMove(2, P1);
	EXPECT_EQ(move.isValid(), true);
	EXPECT_EQ(l2, move);

	move = ms.getNextMove(2, P1);
	EXPECT_EQ(move.isValid(), false);
}

TEST_F(MoveSuggesterFixture, TwoMovesDiffPL) {
	Loc l1(1,1);
	arcs(P1, Line3, 1, l1); // length 3, inc

	Loc l2(2,1);
	arcs(P1, Line2, 1, l2); // length 2, inc

	Loc move = ms.getNextMove(2, P1);
	EXPECT_EQ(move.isValid(), true);
	EXPECT_EQ(l1, move);

	move = ms.getNextMove(2, P1);
	EXPECT_EQ(move.isValid(), true);
	EXPECT_EQ(l2, move);

	move = ms.getNextMove(2, P1);
	EXPECT_EQ(move.isValid(), false);
}

TEST_F(MoveSuggesterFixture, SameMoveDiffPL) {
	Loc l1(1,1);
	arcs(P1, Line3, 1, l1); // length 3, inc

	arcs(P1, Line2, 1, l1); // length 2, inc

	Loc move = ms.getNextMove(2, P1);
	EXPECT_EQ(move.isValid(), true);
	EXPECT_EQ(l1, move);

	// Should only suggest l1 once.
	
	move = ms.getNextMove(2, P1);
	EXPECT_EQ(move.isValid(), false);
}

TEST_F(MoveSuggesterFixture, AddAndThenRemoveSameLoc) {
	Loc l1(1,1);
	arcs(P1, Line4, 1, l1); // length 4, inc
	arcs(P1, Line4, -1, l1); // length 4, dec

	Loc move = ms.getNextMove(2, P2);
	EXPECT_EQ(move.isValid(), false);
}

TEST_F(MoveSuggesterFixture, Extend3BeforeExtend2) {
	Loc l1(1,1);
	arcs(P1, Line3, 1, l1);

	Loc l2(2,2);
	arcs(P1, Line2, 1, l2);

	Loc move = ms.getNextMove(2, P1);
	EXPECT_EQ(move.isValid(), true);
	EXPECT_EQ(l1, move);

	move = ms.getNextMove(2, P1);
	EXPECT_EQ(move.isValid(), true);
	EXPECT_EQ(l2, move);

	move = ms.getNextMove(2, P1);
	EXPECT_EQ(move.isValid(), false);
}

TEST_F(MoveSuggesterFixture, ExtendOursBeforeTheirs) {
	Loc l1(1,1);
	arcs(P1, Line3, 1, l1);

	Loc l2(2,2);
	arcs(P2, Line3, 1, l2);

	Loc move = ms.getNextMove(0, P1);
	EXPECT_EQ(move.isValid(), true);
	EXPECT_EQ(l1, move);

	move = ms.getNextMove(0, P1);
	EXPECT_EQ(move.isValid(), true);
	EXPECT_EQ(l2, move);

	move = ms.getNextMove(0, P1);
	EXPECT_EQ(move.isValid(), false);
}

TEST_F(MoveSuggesterFixture, ExtendTheirsBeforeOurs) {
	Loc l1(1,1);
	arcs(P1, Line3, 1, l1);

	Loc l2(2,2);
	arcs(P2, Line3, 1, l2);

	Loc move = ms.getNextMove(1, P1);
	EXPECT_EQ(move.isValid(), true);
	EXPECT_EQ(l2, move);

	move = ms.getNextMove(1, P1);
	EXPECT_EQ(move.isValid(), true);
	EXPECT_EQ(l1, move);

	move = ms.getNextMove(1, P1);
	EXPECT_EQ(move.isValid(), false);
}

TEST_F(MoveSuggesterFixture, IterateTwiceNoChange) {
	Loc l1(1,1);
	arcs(P1, Line3, 1, l1);

	Loc l2(2,2);
	arcs(P2, Line3, 1, l2);

	Loc move = ms.getNextMove(2, P1);
	EXPECT_EQ(l1, move);

	move = ms.getNextMove(2, P1);
	EXPECT_EQ(l2, move);

	move = ms.getNextMove(2, P1);
	EXPECT_EQ(move.isValid(), false);

	move = ms.getNextMove(2, P1);
	EXPECT_EQ(move.isValid(), false);

	move = ms.getNextMove(2, P1);
	EXPECT_EQ(move.isValid(), false);

	move = ms.getNextMove(2, P1);
	EXPECT_EQ(move.isValid(), false);
}

TEST_F(MoveSuggesterFixture, IterateDifferentDepths) {
	Loc l1(1,1);
	Loc l2(17,1);
	arcs(P1, Line1, 1, l1);
	arcs(P1, Line1, 1, l2);

	Loc move = ms.getNextMove(1, P1);
	EXPECT_EQ(move.isValid(), true); // Don't care which came first

	// Since this is only the move suggestor, it could even suggest the same
	// loc for P2. In the full AI, making the move updates the candidates.
	move = ms.getNextMove(2, P2);
	EXPECT_EQ(move.isValid(), true);
}

TEST_F(MoveSuggesterFixture, dontStartInTheMiddle13)
{
	// Starting in the middle of the board is not handled here.
	LocArr la = getLocsInOrder(0);
	EXPECT_EQ((UL)0, la.size());
}

TEST_F(MoveSuggesterFixture, AddAndRemove)
{
	arcs(P1, Line4, 1, Loc(3,4));
	arcs(P1, Line4, -1, Loc(3,4));
	arcs(P1, Line3, 1, Loc(3,2));
	LocArr la = getLocsInOrder(0);
	EXPECT_EQ((UL)1, la.size());
	EXPECT_EQ(Loc(3,2), la[0]);
}

TEST_F(MoveSuggesterFixture, IterateOverOurFour)
{
	// Don't bother with the 3->4 as we can win this turn with the 4->5
	arcs(P1, Line4, 1, Loc(3,4));
	arcs(P1, Line3, 1, Loc(3,2));
	LocArr la = getLocsInOrder(0);
	EXPECT_EQ((UL)1, la.size());
	EXPECT_EQ(Loc(3,4), la[0]);
}

#if 0
TEST_F(MoveSuggesterFixture, iterate_over_one_of_their_fours)
{
	arcs(P2, Line4, 1, Loc(3,4));
	arcs(P1, Take, 1, Loc(3,2));
	setCapturedBy(P1, 6);
	LocArr la = getLocsInOrder(0);
	EXPECT_EQ((UL)2, la.size());
	EXPECT_EQ(Loc(3,4),la[0]);
	EXPECT_EQ(Loc(3,2),la[1]);
}

TEST_F(MoveSuggesterFixture, test_two_of_their_fours_try_the_take_only)
{
	arcs(P2, Line4, 1, Loc(1,2));
	arcs(P2, Line4, 1, Loc(3,4));
	arcs(P1, Take, 1, Loc(3,2));
	LocArr la = getLocsInOrder(0);
	EXPECT_EQ((UL)1, la.size());
	EXPECT_EQ(Loc(3,2), la[0]);
}
#endif

TEST_F(MoveSuggesterFixture, test_two_of_their_fours_no_take)
{
	arcs(P2, Line4, 1, Loc(1,2));
	arcs(P2, Line4, 1, Loc(3,4));
	LocArr la = getLocsInOrder(0);
	EXPECT_EQ((UL)1, la.size());
	// It doesn't matter which one we choose, we're lost
	// Evaluating this node should give the result
	// But we need to choose one or the other
}


TEST_F(MoveSuggesterFixture, test_finish_capture_win)
{
	setCapturedBy(P1, 8);
	arcs(P1, Take, 1, Loc(1,2));
	arcs(P2, Line4, 1, Loc(3,4));
	LocArr la = getLocsInOrder(0);
	EXPECT_EQ((UL)1, la.size());
	EXPECT_EQ(Loc(1,2), la[0]);
}

#if 0
TEST_F(MoveSuggesterFixture, test_block_or_take_to_defend_capture_loss)
{
	setCapturedBy(P2, 8);
	arcs(P1, Take, 1, Loc(1,2));
	arcs(P2, Take, 1, Loc(3,4));
	LocArr la = getLocsInOrder(0);
	EXPECT_EQ((UL)2, la.size());
}
#endif

TEST_F(MoveSuggesterFixture, test_iterate_over_own_black_first)
{
	arcs(P2, Line4, 1, Loc(1,5));
	arcs(P1, Line4, 1, Loc(3,4));
	LocArr la = getLocsInOrder(0);
	EXPECT_EQ((UL)1, la.size());
	EXPECT_EQ(Loc(3,4), la[0]);
}

TEST_F(MoveSuggesterFixture, test_iterate_over_higher_priority_only)
{
	arcs(P2, Line3, 1, Loc(1,5));
	arcs(P2, Line4, 1, Loc(3,4));
	LocArr la = getLocsInOrder(0);
	EXPECT_EQ((UL)1, la.size());
	EXPECT_EQ(Loc(3,4), la[0]);
}

TEST_F(MoveSuggesterFixture, test_iterate_over_capture)
{
	arcs(P1, Take, 1, Loc(3,4));
	LocArr la = getLocsInOrder(0);
	EXPECT_EQ((UL)1, la.size());
	EXPECT_EQ(Loc(3,4), la[0]);
}

TEST_F(MoveSuggesterFixture, test_iterate_over_own_capture_first)
{
	arcs(P1, Take, 1, Loc(1,2));
	arcs(P2, Take, 1, Loc(3,4));
	LocArr la = getLocsInOrder(1);
	EXPECT_EQ((UL)2, la.size());
	EXPECT_EQ(Loc(3,4), la[0]);
	EXPECT_EQ(Loc(1,2), la[1]);
}

#if 0
TEST_F(MoveSuggesterFixture, test_iterate_over_other_players_four_before_our_capture)
{
	arcs(P2, Take, 1, Loc(7,2));
	arcs(P1, Line4, 1, Loc(3,4));
	LocArr la = getLocsInOrder(1);
	EXPECT_EQ((UL)2, la.size());
	EXPECT_EQ(Loc(3,4), la[0]);
	EXPECT_EQ(Loc(7,2), la[1]);
}
#endif

TEST_F(MoveSuggesterFixture, test_iterate_over_other_players_capture_before_our_threes)
{
	arcs(P1, Line3, 1, Loc(3,4), Loc(1,5));
	arcs(P2, Take, 1, Loc(7,2));
	LocArr la = getLocsInOrder(1);
	EXPECT_EQ((UL)3, la.size());
	EXPECT_EQ(Loc(7,2), la[0]);

	LocSet ourThrees(Loc(3,4), Loc(1,5));
	EXPECT_TRUE(ourThrees.contains(la[1]));
	EXPECT_TRUE(ourThrees.contains(la[2]));
}

#if 0
TEST_F(MoveSuggesterFixture, test_iterate_block_or_capture)
{
	arcs(P2, Line3, 1, Loc(1,1),Loc(2,2));
	arcs(P2, Take, 1, Loc(3,3));
	arcs(P1, Line4, 1, Loc(4,4));
	LocArr la = getLocsInOrder(3);
	EXPECT_EQ((UL)2, la.size());
	EXPECT_EQ(Loc(4,4), la[0]);
	EXPECT_EQ(Loc(3,3), la[1]);
}
#endif

TEST_F(MoveSuggesterFixture, test_iterate_over_their_capture_before_our_two)
{
	arcs(P1, Line2, 1, Loc(2,4), Loc(4,6), Loc(5,7));
	arcs(P2, Take, 1, Loc(1,5));
	LocArr la = getLocsInOrder(0);
	EXPECT_EQ((UL)4, la.size());
	EXPECT_EQ(Loc(1,5), la[0]);

	LocSet ourTwos(Loc(2,4), Loc(4,6), Loc(5,7));
	EXPECT_TRUE(ourTwos.contains(la[1]));
	EXPECT_TRUE(ourTwos.contains(la[2]));
	EXPECT_TRUE(ourTwos.contains(la[3]));
}

TEST_F(MoveSuggesterFixture, test_iterate_over_their_three_before_our_threat)
{
	arcs(P1, Line3, 1, Loc(2,4), Loc(4,6));
	arcs(P2, Threat, 1, Loc(1,5));
	LocArr la = getLocsInOrder(0);
	EXPECT_EQ((UL)3, la.size());

	LocSet threes(Loc(2,4), Loc(4,6));
	EXPECT_TRUE(threes.contains(la[0]));
	EXPECT_TRUE(threes.contains(la[1]));
	
	EXPECT_EQ(Loc(1,5), la[2]);
}

TEST_F(MoveSuggesterFixture, test_add_and_remove_length_candidate)
{
	arcs(P1, Line3, 1, Loc(2,4), Loc(4,6));
	arcs(P1, Threat, 1, Loc(1,5));
	arcs(P1, Line3,-1, Loc(2,4), Loc(4,6));
	LocArr la = getLocsInOrder(0);
	ASSERT_EQ((UL)1, la.size());
	EXPECT_EQ(Loc(1,5), la[0]);
}

TEST_F(MoveSuggesterFixture, test_add_and_remove_capture_candidate)
{
	arcs(P1, Take, 1, Loc(1,5));
	arcs(P1, Take, -1, Loc(1,5));
	LocArr la = getLocsInOrder(0);
	EXPECT_EQ((UL)0, la.size());
}

TEST_F(MoveSuggesterFixture, test_add_and_remove_threat_candidate)
{
	arcs(P1, Threat, 1, Loc(1,5));
	arcs(P1, Threat, -1, Loc(1,5));
	LocArr la = getLocsInOrder(0);
	EXPECT_EQ((UL)0, la.size());
}

TEST_F(MoveSuggesterFixture, test_add_and_remove_length_candidate_from_diff_directions)
{
	arcs(P1, Line3, 1, Loc(2,4), Loc(4,6));
	arcs(P1, Line3, 1, Loc(2,4), Loc(3,3));
	arcs(P1, Line3,-1, Loc(2,4), Loc(4,6));
	LocArr la = getLocsInOrder(0);
	EXPECT_EQ((UL)2, la.size());
	LocSet okPair(Loc(2,4), Loc(3,3));
	EXPECT_TRUE(okPair.contains(la[0]));
	EXPECT_TRUE(okPair.contains(la[1]));
}

TEST_F(MoveSuggesterFixture, test_multiple_entries_searched_first)
{
	arcs(P1, Line3, 1, Loc(2,4), Loc(4,6));
	arcs(P1, Line3, 1, Loc(2,4), Loc(3,3));
	LocArr la = getLocsInOrder(0);
	EXPECT_EQ((UL)3, la.size());
	EXPECT_EQ(Loc(2,4), la[0]);
	LocSet others(Loc(4,6), Loc(3,3));
	EXPECT_TRUE(others.contains(la[1]));
	EXPECT_TRUE(others.contains(la[2]));
}

////////////////////////////////////////////////////////////////////////
// take targetting tests
////////////////////////////////////////////////////////////////////////

// debug/tests --gtest_filter="*takeFourFirst*"
TEST_F(MoveSuggesterFixture, takeFourBeforeBlockingIt)
{
	ps._takeTargeting = true;
	arcs(P2, Line4, 1, Loc(2,4));
	arcs(P1, FourTake, 1, Loc(3,3));
	LocArr la = getLocsInOrder(0);
	EXPECT_EQ((UL)2, la.size());
	EXPECT_EQ(Loc(3,3), la[0]);
	EXPECT_EQ(Loc(2,4), la[1]);
}

TEST_F(MoveSuggesterFixture, OnlyTakeFourWhenThereAreTwoFours)
{
	ps._takeTargeting = true;
	arcs(P2, Line4, 1, Loc(2,4));
	arcs(P2, Line4, 1, Loc(3,4));
	arcs(P1, FourTake, 1, Loc(3,3));
	LocArr la = getLocsInOrder(0);
	EXPECT_EQ((UL)1, la.size());
	EXPECT_EQ(Loc(3,3), la[0]);
}

TEST_F(MoveSuggesterFixture, TakeTakeBeforeBlockTake)
{
	ps._takeTargeting = true;
	setCapturedBy(P2, 8);
	arcs(P2, Take, 1, Loc(2,4));
	arcs(P1, TakeTake, 1, Loc(3,3));
	LocArr la = getLocsInOrder(0);
	// But try both.
	EXPECT_EQ((UL)2, la.size());
	EXPECT_EQ(Loc(3,3), la[0]);
	EXPECT_EQ(Loc(2,4), la[1]);
}

#if 0
// TODO
TEST_F(MoveSuggesterFixture, OnlyTakeTakeIfThereAreTwoTakeWins)
{
	ps._takeTargeting = true;
	setCapturedBy(P2, 8);
	arcs(P2, Take, 1, Loc(2,4));
	arcs(P2, Take, 1, Loc(3,4));
	arcs(P1, TakeTake, 1, Loc(3,3));
	LocArr la = getLocsInOrder(0);
	EXPECT_EQ((UL)1, la.size());
	EXPECT_EQ(Loc(3,3), la[0]);
}
#endif

#if 0
// TODO?
TEST_F(MoveSuggesterFixture, takeFourBlockerBeforeExtendThreeOrArbTake)
{
	ps._takeTargeting = true;
	arcs(P1, Line3, 1, Loc(2,4));
	arcs(P1, Take, 1, Loc(1,5));
	arcs(P1, Blocked4Take, 1, Loc(3,3));
	LocArr la = getLocsInOrder(0);
	EXPECT_EQ((UL)3, la.size());
	EXPECT_EQ(Loc(3,3), la[0]);
	EXPECT_EQ(Loc(1,5), la[1]);
	EXPECT_EQ(Loc(2,4), la[2]);
}
#endif

// Other tests

TEST_F(MoveSuggesterFixture, test_choose_the_only_winner)
{
	arcs(P1, Line4, 1, Loc(4,6));
	arcs(P1, Line4, 1, Loc(5,7));
	arcs(P1, Line4,-1, Loc(4,6));
	LocArr la = getLocsInOrder(0);
	EXPECT_EQ((UL)1, la.size());
	EXPECT_EQ(Loc(5,7), la[0]);
}

TEST_F(MoveSuggesterFixture, test_only_one_option)
{
	arcs(P1, Line4, 1, Loc(4,6));
	
	bool onlyOne = ms.isOnlyOneMove(0, P2);
	EXPECT_EQ(true, onlyOne);
	LocArr la = getLocsInOrder(0);
	EXPECT_EQ((UL)1, la.size());
	EXPECT_EQ(Loc(4,6), la[0]);
}

#include <set>
using namespace std;

#if 0
TEST_F(MoveSuggesterFixture, test_one_opponent_double_three_must_be_blocked_capped_or_threatened)
{
	// i.e. a single instance of a double 3 attack must be blocked,
	// captured, or threatened, or we must extend a 3 of our own
	// Blocking ONE of the lines on the other candidate may be enough.
	
	// all our 3s should be included (first)
	arcs(P2, Line3, 1, Loc(4,8), Loc(10,6));

	// Only their threes need to be looked at, since there is a double 3.
	// i.e. not our twos or their twos. (second)
	arcs(P1, Line3, 1, Loc(4,6), Loc(5,6));
	arcs(P1, Line3, 1, Loc(5,6), Loc(9,6));

	// Then our take
	arcs(P2, Take, 1, Loc(1,5));

	// And our threat
	// Only threats that threaten the double 3 really need considering (TODO)
	arcs(P2, Threat, 1, Loc(2,9));

	// All 2s are irrelevant since there is a double 3
	arcs(P2, Line2, 1, Loc(7,8), Loc(8,8), Loc(10,8));
	arcs(P2, Line2, 1, Loc(8,8), Loc(10,8), Loc(12,8));
	arcs(P2, Line2, 1, Loc(10,8), Loc(12,8), Loc(13,8));

	LocArr la = getLocsInOrder(1);

	EXPECT_EQ((UL)7, la.size());
	set<Loc> our_threes;
	our_threes.insert(Loc(4,8));
	our_threes.insert(Loc(10,6)); // Our 3s first
	EXPECT_EQ((UL)1, our_threes.count(la[0]));
	EXPECT_EQ((UL)1, our_threes.count(la[1]));

	EXPECT_EQ(Loc(5,6), la[2]); // Their double 3 next 

	set<Loc> their_threes; // Then single 3s
	their_threes.insert(Loc(4,6));
	their_threes.insert(Loc(9,6));
	EXPECT_EQ((UL)1, their_threes.count(la[3]));
	EXPECT_EQ((UL)1, their_threes.count(la[4]));

	// Then our take
	EXPECT_EQ(Loc(1,5), la[5]);

	// And our threat
	// Only threats that threaten the double 3 really need considering (TODO)
	EXPECT_EQ(la[6], Loc(2,9));
}

TEST_F(MoveSuggesterFixture, test_three_plus_opponent_double_threes_cannot_block)
{
	// i.e. three or more double 3 attacks cannot be blocked;
	// we must extend a 3 of our own, capture or threaten

	// 3 x Two double attacks - can't block 'em all
	arcs(P1, Line3, 1, Loc(4,6), Loc(5,6));
	arcs(P1, Line3, 1, Loc(5,6), Loc(9,6));
	arcs(P1, Line3, 1, Loc(4,6), Loc(9,6));

	// takes that are not involved in the double 3s may be useful for reaching
	// 5 pair captures
	arcs(P2, Take, 1, Loc(1,5));

	// all our 3s should be included since we may be able to get five first
	// or block a double 3 at the same time.
	arcs(P2, Line3, 1, Loc(4,8), Loc(10,6));

	// only threats that threaten the double 3s really need considering (TODO)
	arcs(P2, Threat, 1, Loc(2,9));

	// All 2s are irrelevant since there is a double 3
	arcs(P2, Line2, 1, Loc(7,8), Loc(8,8), Loc(10,8));
	arcs(P2, Line2, 1, Loc(8,8), Loc(10,8), Loc(12,8));
	arcs(P2, Line2, 1, Loc(10,8), Loc(12,8), Loc(13,8));

	LocArr la = getLocsInOrder(1);
	EXPECT_EQ((UL)4, la.size());
	set<Loc> our_threes;
	our_threes.insert(Loc(4,8));
	our_threes.insert(Loc(10,6)); // Our 3s
	EXPECT_EQ((UL)1, our_threes.count(la[0]));
	EXPECT_EQ((UL)1, our_threes.count(la[1]));
	EXPECT_EQ(la[2], Loc(1,5)); // Our take
	EXPECT_EQ(la[3], Loc(2,9)); // Our threat
	// TODO: This should also deepen the search (by 1?)
}
#endif
