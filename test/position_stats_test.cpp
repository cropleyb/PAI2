#include "gmock/gmock.h"

#define private public

#include "defines.h"
#include "loc.h"
#include "position_stats.h"

class PositionStatsFixture : public testing::Test {
public:
	PositionStats ps;
	Loc locBuffer[MAX_CANDS];
	Ind countBuffer[MAX_CANDS];
};

using ::testing::AtLeast;
using ::testing::Return;
using ::testing::InSequence;

using std::vector;
typedef vector<Loc> LocArr;

TEST_F(PositionStatsFixture, NoStatsYet) {
	const PriorityLevel &pl = ps.getLengthPriorityLevel(P1, 1);
	EXPECT_EQ(0, pl.getNumCands());

	Ind found = pl.getCands(locBuffer, countBuffer, 4);
	EXPECT_EQ(0, found);
}

TEST_F(PositionStatsFixture, AddSingleP1) {
	LocArr ll;
	Loc l1(5,2);
	ll.push_back(l1);
	ps.reportLengthCandidates(P1, 1, ll, 1); // length 1, inc

	const PriorityLevel &pl = ps.getLengthPriorityLevel(P1, 1);
	EXPECT_EQ(1, pl.getNumCands());

	Ind found = pl.getCands(locBuffer, countBuffer, 4); // Max
	EXPECT_EQ(1, found);
	EXPECT_EQ(l1, locBuffer[0]);
	EXPECT_EQ(1, countBuffer[0]);
}

TEST_F(PositionStatsFixture, AddThreeP2s) {
	LocArr ll;
	Loc l1(1,1);
	Loc l2(2,2);
	Loc l3(3,3);
	ll.push_back(l1);
	ll.push_back(l2);
	ll.push_back(l3);
	ps.reportLengthCandidates(P2, 2, ll, 1); // length 1, inc

	const PriorityLevel &pl1 = ps.getLengthPriorityLevel(P1, 1);
	EXPECT_EQ(0, pl1.getNumCands());
	const PriorityLevel &pl2 = ps.getLengthPriorityLevel(P2, 2);

	Ind found = pl2.getCands(locBuffer, countBuffer, 4); // Max
	EXPECT_EQ(3, found);
	EXPECT_EQ(1, countBuffer[0]);
	EXPECT_EQ(1, countBuffer[1]);
	EXPECT_EQ(1, countBuffer[2]);

	// FIXME: remove order dependence
	EXPECT_EQ(l3, locBuffer[0]);
	EXPECT_EQ(l2, locBuffer[1]);
	EXPECT_EQ(l1, locBuffer[2]);
}

TEST_F(PositionStatsFixture, AddAndRemoveSingleP1) {
	LocArr ll;
	Loc l1(5,2);
	ll.push_back(l1);
	ps.reportLengthCandidates(P1, 1, ll, 1); // length 1, inc
	ps.reportLengthCandidates(P1, 1, ll, -1); // length 1, dec

	const PriorityLevel &pl = ps.getLengthPriorityLevel(P1, 1);
	EXPECT_EQ(0, pl.getNumCands());

	Ind found = pl.getCands(locBuffer, countBuffer, 4); // Max
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
	Loc l1(5,2);
	ps.reportTake(P1, l1, 1);

	const PriorityLevel &pl1 = ps.getTakesPriorityLevel(P1);
	EXPECT_EQ(1, pl1.getNumCands());
	const PriorityLevel &pl2 = ps.getTakesPriorityLevel(P2);
	EXPECT_EQ(0, pl2.getNumCands());
}
