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

	LocArr getLocsInOrder(Depth d)
	{
		LocArr la;
		for (Loc loc=ms.getNextMove(d); loc.isValid(); loc=ms.getNextMove(d)) {
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
	void arcs(Colour c, Length l, int inc, Types ... rest)
	{
		LocArr ll;
		addLocs(ll, rest...);
		ps.reportLengthCandidates(c, l, ll, inc);
	}

	// Hack for testing...
    void setCapturedBy(Colour colour, int count)
	{
		ps._captured[colour] = count;
	}

    void arTake(Colour c, int inc, Loc l)
	{
		ps._takes[c].addOrRemoveCandidate(l, inc);
	}

    void arThreat(Colour c, Loc l, int inc)
	{
		ps._threats[c].addOrRemoveCandidate(l, inc);
	}
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
	arcs(P1, 3, 1, l2, l1, l1); // length 2, inc

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
	arcs(P1, 3, 1, l1); // length 2, inc

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

TEST_F(MoveSuggesterFixture, dontStartInTheMiddle13)
{
	// Starting in the middle of the board is not handled here.
	LocArr la = getLocsInOrder(0);
	EXPECT_EQ(0, la.size());
}

TEST_F(MoveSuggesterFixture, AddAndRemove)
{
	arcs(P1, 4, 1, Loc(3,4));
	arcs(P1, 4, -1, Loc(3,4));
	arcs(P1, 3, 1, Loc(3,2));
	LocArr la = getLocsInOrder(0);
	EXPECT_EQ(1, la.size());
	EXPECT_EQ(Loc(3,2), la[0]);
}

TEST_F(MoveSuggesterFixture, IterateOverOurFour)
{
	// Don't bother with the 3->4 as we can win this turn with the 4->5
	arcs(P1, 4, 1, Loc(3,4));
	arcs(P1, 3, 1, Loc(3,2));
	LocArr la = getLocsInOrder(0);
	EXPECT_EQ(1, la.size());
	EXPECT_EQ(Loc(3,4), la[0]);
}

TEST_F(MoveSuggesterFixture, iterate_over_one_of_their_fours)
{
	arcs(P2, 4, 1, Loc(3,4));
	arTake(P1, 1, Loc(3,2));
	setCapturedBy(P1, 6);
	LocArr la = getLocsInOrder(0);
	EXPECT_EQ(2, la.size());
	EXPECT_EQ(Loc(3,4),la[0]);
	EXPECT_EQ(Loc(3,2),la[1]);
}

TEST_F(MoveSuggesterFixture, test_two_of_their_fours_try_the_take)
{
	arcs(P2, 4, 1, Loc(1,2));
	arcs(P2, 4, 1, Loc(3,4));
	arTake(P1, 1, Loc(3,2));
	LocArr la = getLocsInOrder(0);
	EXPECT_EQ(la.size(), 1);
	EXPECT_EQ(Loc(3,2), la[0]);
}

TEST_F(MoveSuggesterFixture, test_two_of_their_fours_no_take)
{
	arcs(P2, 4, 1, Loc(1,2));
	arcs(P2, 4, 1, Loc(3,4));
	LocArr la = getLocsInOrder(0);
	EXPECT_EQ(la.size(), 1);
	// It doesn't matter which one we choose, we're lost
	// Evaluating this node should give the result
	// But we need to choose one or the other
}


TEST_F(MoveSuggesterFixture, test_finish_capture_win)
{
	setCapturedBy(P1, 8);
	arTake(P1, 1, Loc(1,2));
	arcs(P2, 4, 1, Loc(3,4));
	LocArr la = getLocsInOrder(0);
	EXPECT_EQ(1, la.size());
	EXPECT_EQ(Loc(1,2), la[0]);
}
#if 0

TEST_F(MoveSuggesterFixture, test_block_or_take_to_defend_capture_loss)
{
	setCapturedBy(P2, 8);
	arTake(P1, 1, Loc(1,2));
	arTake(P2, 1, Loc(3,4));
	LocArr la = getLocsInOrder(0);
	EXPECT_EQ(la.size(), 2);
}

TEST_F(MoveSuggesterFixture, test_iterate_over_own_black_first)
{
	arcs(P2, 4, 1, Loc(1,5));
	arcs(P1, 4, 1, Loc(3,4));
	LocArr la = getLocsInOrder(0);
	EXPECT_EQ(la.size(), 1);
	EXPECT_EQ(la[0],(3,4));
}

TEST_F(MoveSuggesterFixture, test_iterate_over_higher_priority_only)
{
	arcs(P2, 3, 1, Loc(1,5));
	arcs(P2, 4, 1, Loc(3,4));
	LocArr la = getLocsInOrder(0);
	EXPECT_EQ(la.size(), 1);
	EXPECT_EQ(la[0],(3,4));
}

TEST_F(MoveSuggesterFixture, test_iterate_over_capture)
{
	pf.add_or_remove_take(P1, Loc(3,4));
	LocArr la = getLocsInOrder(0);
	EXPECT_EQ(la.size(), 1);
	EXPECT_EQ(la[0],(3,4));
}

TEST_F(MoveSuggesterFixture, test_iterate_over_own_capture_first)
{
	pf.add_or_remove_take(P1, Loc(1,2));
	pf.add_or_remove_take(P2, Loc(3,4));
	l = list(pf.get_iter(P2));
	EXPECT_EQ(la.size(), 2);
	EXPECT_EQ(la[0],(3,4));
	EXPECT_EQ(la[1],(1,2));
}

TEST_F(MoveSuggesterFixture, test_iterate_over_other_players_four_before_our_capture)
{
	pf.add_or_remove_take(P2, Loc(7,2));
	arcs(P1, 4, 1, Loc(3,4));
	l = list(pf.get_iter(P2));
	EXPECT_EQ(la.size(), 2);
	EXPECT_EQ(la[0],(3,4));
	EXPECT_EQ(la[1],(7,2));
}

TEST_F(MoveSuggesterFixture, atest_iterate_over_other_players_capture_before_our_threes)
{
	arcs(P1, 3, 1, Loc(3,4),(1,5)));
	pf.add_or_remove_take(P2, Loc(7,2));
	l = list(pf.get_iter(P2));
	EXPECT_EQ(la.size(), 3);
	EXPECT_EQ(la[0],(7,2));
	our_threes = Loc(3,4),(1,5));
	assertIn(la[1], our_threes);
	assertIn(la[2], our_threes);
}

TEST_F(MoveSuggesterFixture, test_iterate_block_only)
{
	arcs(P2, 3, 1, Loc(1,5),(2,4)));
	pf.add_or_remove_take(P1, Loc(1,5));
	arcs(P1, 4, 1, Loc(2,4));
	l = list(pf.get_iter(P2));
	EXPECT_EQ(la.size(), 1);
	EXPECT_EQ(la[0],(2,4));
}

TEST_F(MoveSuggesterFixture, test_iterate_over_capture)
{
	pf.add_or_remove_take(P1, Loc(1,5));
	l = list(pf.get_iter(P2));
	EXPECT_EQ(la.size(), 1);
	EXPECT_EQ(l[0],(1,5));
}

TEST_F(MoveSuggesterFixture, test_iterate_over_their_capture_before_our_two)
{
	arcs(P1, 2, 1, Loc(2,4),(4,6),(5,7)));
	pf.add_or_remove_take(P2, Loc(1,5));
	LocArr la = getLocsInOrder(0);
	EXPECT_EQ(la.size(), 4);
	EXPECT_EQ(l[0],(1,5));
	twos = (2,4),(4,6),(5,7);
	assertIn(l[1], twos);
	assertIn(l[2], twos);
	assertIn(l[3], twos);
}

TEST_F(MoveSuggesterFixture, test_iterate_over_their_three_before_our_threat)
{
	arcs(P1, 3, 1, Loc(2,4),(4,6));
	pf.add_or_remove_threat(P2, Loc(1,5));
	LocArr la = getLocsInOrder(0);
	EXPECT_EQ(la.size(), 3);
	threes = (2,4),(4,6);
	assertIn(l[0], threes);
	assertIn(l[1], threes);
	EXPECT_EQ(l[2],(1,5));
	
TEST_F(MoveSuggesterFixture, test_add_and_remove_length_candidate)
{
	#st();
	arcs(P1, 3, 1, Loc(2,4),(4,6),), inc=1);
	pf.add_or_remove_threat(P1, Loc(1,5));
	arcs(P1, 3, 1, Loc(2,4),(4,6),), inc=-1);
	LocArr la = getLocsInOrder(0);
	EXPECT_EQ(la.size(), 1);
	EXPECT_EQ(l[0],(1,5));
}

TEST_F(MoveSuggesterFixture, test_add_and_remove_capture_candidate)
{
	pf.add_or_remove_take(P1, Loc(1,5), inc=1);
	pf.add_or_remove_take(P1, Loc(1,5), inc=-1);
	LocArr la = getLocsInOrder(0);
	EXPECT_EQ(la.size(), 0);
}

TEST_F(MoveSuggesterFixture, test_add_and_remove_threat_candidate)
{
	pf.add_or_remove_threat(P1, Loc(1,5), inc=1);
	pf.add_or_remove_threat(P1, Loc(1,5), inc=-1);
	LocArr la = getLocsInOrder(0);
	EXPECT_EQ(la.size(), 0);
}

TEST_F(MoveSuggesterFixture, test_add_and_remove_length_candidate_from_diff_directions)
{
	arcs(P1, 3, 1, Loc(2,4),(4,6));
	arcs(P1, 3, 1, Loc(2,4),(3,3));
	arcs(P1, 3,-1, Loc(2,4),(4,6));
	LocArr la = getLocsInOrder(0);
	EXPECT_EQ(la.size(), 2);
	pair = Loc(2,4),(3,3),);
	assertIn(l[0], pair);
	assertIn(l[1], pair);
}

TEST_F(MoveSuggesterFixture, test_multiple_entries_searcshed_first)
{
	arcs(P1, 3, 1, Loc(2,4),(4,6));
	arcs(P1, 3, 1, Loc(2,4),(3,3));
	LocArr la = getLocsInOrder(0);
	EXPECT_EQ(la.size(), 3);
	EXPECT_EQ(l[0],(2,4));
	others = Loc(4,6), Loc(3,3));
	assertIn(l[1], others);
	assertIn(l[2], others);
}

TEST_F(MoveSuggesterFixture, test_choose_the_only_winner)
{
	arcs(P1, 4, 1, Loc(4,6));
	arcs(P1, 4, 1, Loc(5,7));
	arcs(P1, 4,-1, Loc(4,6));
	LocArr la = getLocsInOrder(0);
	EXPECT_EQ(la.size(), 1);
	EXPECT_EQ(l[0],(5,7));
}

TEST_F(MoveSuggesterFixture, atest_one_opponent_double_three_must_be_block_cap_or_threatened)
{
	# i.e. a single instance of a double 3 attack must be blocked,
	# captured, or threatened, or we must extend a 3 of our own
	pf.add_or_remove_take(P2, Loc(1,5), inc=1);

	# Only their threes needs to be looked at, since there is a double 3
	arcs(P1, 3, 1, Loc(4,6),(5,6));
	arcs(P1, 3, 1, Loc(5,6),(9,6));

	# all our 3s should be included
	arcs(P2, 3,-1, Loc(4,8),(10,6));

	# only threats that threaten the double 3 really need considering (TODO)
	pf.add_or_remove_threat(P2, Loc(2,9), inc=1);

	# All 2s are irrelevant since there is a double 3
	arcs(P2, 2, 1, Loc(7,8),(8,8),(10,8)));
	arcs(P2, 2, 1, Loc(8,8),(10,8),(12,8)));
	arcs(P2, 2, 1, Loc(10,8),(12,8),(13,8)));

	#st()
	l = list(pf.get_iter(P2));

	EXPECT_EQ(la.size(), 5);
	EXPECT_EQ(l[0], Loc(5,6)); # Their open 3
	EXPECT_EQ(l[1], Loc(1,5)); # Our take
	our_threes = Loc(4,8), Loc(10,6)); # Our 3s
	assertIn(l[2], our_threes);
	assertIn(l[3], our_threes);
	EXPECT_EQ(l[4], Loc(2,9)); # Our threat (TODO)
}

TEST_F(MoveSuggesterFixture, atest_three_plus_opponent_double_threes_cannot_block)
{
	# i.e. a single instance of a double 3 attack must be blocked,
	# captured, or threatened, or we must extend a 3 of our own
	pf.add_or_remove_take(P2, Loc(1,5), inc=1)

	# 3 x Two double attacks - can't block
	arcs(P1, 3, 1, Loc(4,6),(5,6));
	arcs(P1, 3, 1, Loc(5,6),(9,6));
	arcs(P1, 3, 1, Loc(4,6),(9,6));

	# all our 3s should be included
	arcs(P2, 3, 1, Loc(4,8),(10,6));

	# only threats that threaten the double 3 really need considering (TODO)
	pf.add_or_remove_threat(P2, Loc(2,9), inc=1);

	# All 2s are irrelevant since there is a double 3
	arcs(P2, 2, 1, Loc(7,8),(8,8),(10,8)));
	arcs(P2, 2, 1, Loc(8,8),(10,8),(12,8)));
	arcs(P2, 2, 1, Loc(10,8),(12,8),(13,8)));

	l = list(pf.get_iter(P2));
	EXPECT_EQ(la.size(), 4);
	EXPECT_EQ(l[0], Loc(1,5)); # Our take
	our_threes = Loc(4,8), Loc(10,6)); # Our 3s
	assertIn(l[1], our_threes);
	assertIn(l[2], our_threes);
	EXPECT_EQ(l[3], Loc(2,9)); # Our threat (TODO)
}

#endif

