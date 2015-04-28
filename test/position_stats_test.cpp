#include "gmock/gmock.h"

#define private public

#include "defines.h"
#include "loc.h"
#include "position_stats.h"

class PositionStatsFixture : public testing::Test {
public:
	PositionStats ps;
	Loc locBuffer[MAX_CANDS];
	U64 seen[MAX_WIDTH] = { 0 };
};

using ::testing::AtLeast;
using ::testing::Return;
using ::testing::InSequence;

using std::vector;
typedef vector<Loc> LocArr;

TEST_F(PositionStatsFixture, NoStatsYet) {
	const PriorityLevel &pl = ps.getPriorityLevel(P1, Line1);
	EXPECT_EQ(0, pl.getNumCands());

	Ind found = pl.getCands(locBuffer, 4, seen);
	EXPECT_EQ(0, found);
}

TEST_F(PositionStatsFixture, AddSingleP1) {
	LocArr ll;
	Loc l1(5,2);
	ll.push_back(l1);
	ps.reportCandidates(P1, Line1, ll, 1); // length 1, inc

	const PriorityLevel &pl = ps.getPriorityLevel(P1, Line1);
	EXPECT_EQ(1, pl.getNumCands());

	Ind found = pl.getCands(locBuffer, 4, seen); // Max
	EXPECT_EQ(1, found);
	EXPECT_EQ(l1, locBuffer[0]);
}

TEST_F(PositionStatsFixture, AddThreeP2s) {
	LocArr ll;
	Loc l1(1,1);
	Loc l2(2,2);
	Loc l3(3,3);
	ll.push_back(l1);
	ll.push_back(l2);
	ll.push_back(l3);
	ps.reportCandidates(P2, Line2, ll, 1); // length 1, inc

	const PriorityLevel &pl1 = ps.getPriorityLevel(P1, Line1);
	EXPECT_EQ(0, pl1.getNumCands());
	const PriorityLevel &pl2 = ps.getPriorityLevel(P2, Line2);

	Ind found = pl2.getCands(locBuffer, 4, seen); // Max
	EXPECT_EQ(3, found);

	// FIXME: remove order dependence
	EXPECT_EQ(l3, locBuffer[0]);
	EXPECT_EQ(l2, locBuffer[1]);
	EXPECT_EQ(l1, locBuffer[2]);
}

TEST_F(PositionStatsFixture, AddAndRemoveSingleP1) {
	LocArr ll;
	Loc l1(5,2);
	ll.push_back(l1);
	ps.reportCandidates(P1, Line1, ll, 1); // length 1, inc
	ps.reportCandidates(P1, Line1, ll, -1); // length 1, dec

	const PriorityLevel &pl = ps.getPriorityLevel(P1, Line1);
	EXPECT_EQ(0, pl.getNumCands());

	Ind found = pl.getCands(locBuffer, 4, seen); // Max
	EXPECT_EQ(0, found);
}

TEST_F(PositionStatsFixture, AddP1Captured) {
	ps.reportCaptured(P1, 2, 1); // count, inc

	int capturedP1 = ps.getCaptured(P1);
	EXPECT_EQ(2, capturedP1);
	int capturedP2 = ps.getCaptured(P2);
	EXPECT_EQ(0, capturedP2);
}

TEST_F(PositionStatsFixture, AddAndRemoveP2Captured) {
	ps.reportCaptured(P2, 3, 1);
	ps.reportCaptured(P2, 2, 1);
	ps.reportCaptured(P2, 2, -1);

	int capturedP1 = ps.getCaptured(P1);
	EXPECT_EQ(0, capturedP1);
	int capturedP2 = ps.getCaptured(P2);
	EXPECT_EQ(3, capturedP2);
}

TEST_F(PositionStatsFixture, AddP1Take) {
	ps.reportCandidate(P1, Take, Loc(5,2), 1);

	const PriorityLevel &pl1 = ps.getPriorityLevel(P1, Take);
	EXPECT_EQ(1, pl1.getNumCands());
	const PriorityLevel &pl2 = ps.getPriorityLevel(P2, Take);
	EXPECT_EQ(0, pl2.getNumCands());
}

TEST_F(PositionStatsFixture, AddP2Take) {
	ps.reportCandidate(P2, Take, Loc(5,2), 1);

	const PriorityLevel &pl2 = ps.getPriorityLevel(P2, Take);
	EXPECT_EQ(1, pl2.getNumCands());
	const PriorityLevel &pl1 = ps.getPriorityLevel(P1, Take);
	EXPECT_EQ(0, pl1.getNumCands());
}

TEST_F(PositionStatsFixture, AddP1Threat) {
	ps.reportCandidate(P1, Threat, Loc(5,2), 1);

	const PriorityLevel &pl1 = ps.getPriorityLevel(P1, Threat);
	EXPECT_EQ(1, pl1.getNumCands());
	const PriorityLevel &pl2 = ps.getPriorityLevel(P2, Threat);
	EXPECT_EQ(0, pl2.getNumCands());
}

TEST_F(PositionStatsFixture, AddP2Threat) {
	ps.reportCandidate(P2, Threat, Loc(5,2), 1);

	const PriorityLevel &pl2 = ps.getPriorityLevel(P2, Threat);
	EXPECT_EQ(1, pl2.getNumCands());
	const PriorityLevel &pl1 = ps.getPriorityLevel(P1, Threat);
	EXPECT_EQ(0, pl1.getNumCands());
}

TEST_F(PositionStatsFixture, TestPatternCount) {
	ps.reportCandidate(P2, Threat, Loc(5,2), 1);

	PattCount c = ps.getNumPatterns(P2, Threat);
	EXPECT_EQ(1, c);
}

TEST_F(PositionStatsFixture, TestBlockedCount) {
	LocArr ll;
	ps.reportCandidates(P2, Blocked, ll, 1); // inc

	PattCount c = ps.getNumPatterns(P2, Blocked);
	EXPECT_EQ(1, c);
}

/////////////////////////////////////////////////////
// test get moves to win
/////////////////////////////////////////////////////

TEST_F(PositionStatsFixture, TestMovesToWin1byLine4) {
	ps.reportCandidate(P2, Line4, Loc(5,2), 1); // inc

	Breadth n = ps.getMovesToWin(P2);
	EXPECT_EQ(1, n);
}

TEST_F(PositionStatsFixture, TestMovesToWin1byCaps) {
	ps.reportCandidate(P2, Take, Loc(5,2), 1); // inc
	ps._captured[P2] = 8;

	Breadth n = ps.getMovesToWin(P2);
	EXPECT_EQ(1, n);
}

TEST_F(PositionStatsFixture, TestMovesToWin2byLine3) {
	ps.reportCandidate(P2, Threat, Loc(2,3), 1);
	ps._captured[P2] = 8;

	Breadth n = ps.getMovesToWin(P2);
	EXPECT_EQ(2, n);
}

TEST_F(PositionStatsFixture, TestMovesToWin2byThreat) {
	ps.reportCandidate(P1, Line3, Loc(5,2), 1);
	ps.reportCandidate(P2, Line4, Loc(5,3), 1); // P2 MTW is 1, but that shouldn't affect P1 MTW

	Breadth n = ps.getMovesToWin(P1);
	EXPECT_EQ(2, n);
}

//////////////////////////////////////////////////////////////////////
// isForced tests
//////////////////////////////////////////////////////////////////////

TEST_F(PositionStatsFixture, isForcedByTheir4) {
	ps.reportCandidate(P1, Line3, Loc(5,2), 1);
	ps.reportCandidate(P2, Line4, Loc(5,3), 1);

	bool forced = ps.isForced(P1);
	EXPECT_EQ(true, forced);
}

TEST_F(PositionStatsFixture, isForcedByTheir3) {
	ps.reportCandidate(P1, Line3, Loc(5,2), 1);
	ps.reportCandidate(P2, Line3, Loc(5,3), 1);

	bool forced = ps.isForced(P1);
	EXPECT_EQ(false, forced);
}

TEST_F(PositionStatsFixture, isForcedByTheirTake) {
	ps._captured[P2] = 8;
	ps.reportCandidate(P1, Line3, Loc(5,2), 1);
	ps.reportCandidate(P2, Take, Loc(5,3), 1);

	bool forced = ps.isForced(P1);
	EXPECT_EQ(true, forced);
}

TEST_F(PositionStatsFixture, isNotForcedByTheirTake) {
	ps._captured[P2] = 6;
	ps.reportCandidate(P1, Line3, Loc(5,2), 1);
	ps.reportCandidate(P2, Take, Loc(5,3), 1);

	bool forced = ps.isForced(P1);
	EXPECT_EQ(false, forced);
}

TEST_F(PositionStatsFixture, canForceByOurThreat) {
	ps._captured[P1] = 8;
	ps.reportCandidate(P1, Threat, Loc(5,3), 1);
	ps.reportCandidate(P2, Line2, Loc(5,2), 1);

	bool forced = ps.isForced(P1);
	EXPECT_EQ(false, forced);
	bool theyreForced = ps.isForced(P2);
	EXPECT_EQ(true, theyreForced);
}

TEST_F(PositionStatsFixture, canForceByThreeTakes) {
	ps._captured[P1] = 6;
	ps.reportCandidate(P1, Take, Loc(5,3), 1);
	ps.reportCandidate(P1, Take, Loc(7,5), 1);
	ps.reportCandidate(P1, Take, Loc(1,5), 1);
	ps.reportCandidate(P2, Line2, Loc(5,2), 1);

	bool forced = ps.isForced(P1);
	EXPECT_EQ(false, forced);
	bool theyreForced = ps.isForced(P2);
	EXPECT_EQ(true, theyreForced);
}
