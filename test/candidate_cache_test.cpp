#include "gmock/gmock.h"

#define private public

#include "defines.h"
#include "loc.h"
#include "candidate_cache.h"
#include "bdebug.h"

using testing::ElementsAre;

class CandidateCacheFixture : public testing::Test {
public:
	vector<Loc> getMoves(Depth d) {
		vector<Loc> allMoves;
		do
		{
			Loc l = cc.getNextMove(d);
			if (!l.isValid()) break;
			allMoves.push_back(l);
		}
		while (true);
		return allMoves;
	}
	CandidateCache cc;
	Loc locBuffer[MAX_DEPTH][MAX_CANDS];
};

using ::testing::AtLeast;
using ::testing::Return;
using ::testing::InSequence;

TEST_F(CandidateCacheFixture, NoMovesYet) {
	Loc l = cc.getNextMove(0);

	EXPECT_EQ(Loc::INVALID, l);
}

TEST_F(CandidateCacheFixture, GetDepth0Buffer) {
	Loc l = Loc(4,7);
    Loc *locs = cc.getBuffer(0);
	locs[0] = l;
	cc.setDepthMoves(0,1);
	Loc l2 = cc.getNextMove(0);

	EXPECT_EQ(l, l2);
}

TEST_F(CandidateCacheFixture, GetMovesFrom2DepthLevels) {
	Loc l1 = Loc(1,1);
	Loc l2 = Loc(2,2);
	Loc l3 = Loc(3,3);
	Loc l4 = Loc(4,4);
	Loc l5 = Loc(5,5);
	Loc l6 = Loc(6,6);
    Loc *locs0 = cc.getBuffer(0);
	BD(cout << "Before: " << locs0[0]._value << endl);
	locs0[0] = l1;
	locs0[1] = l2;
	locs0[2] = l3;
	BD(cout << "After: " << locs0[0]._value << endl);
	cc.setDepthMoves(0,3);
    Loc *locs1 = cc.getBuffer(1);
	locs1[0] = l4;
	locs1[1] = l5;
	locs1[2] = l6;
	cc.setDepthMoves(1,3);

	vector<Loc> res_locs1 = getMoves(1);
	ASSERT_THAT(res_locs1, ElementsAre(l4,l5,l6));
	vector<Loc> res_locs0 = getMoves(0);
	ASSERT_THAT(res_locs0, ElementsAre(l1,l2,l3));
	vector<Loc> res_locs0_again = getMoves(0);
	ASSERT_THAT(res_locs0_again, ElementsAre());
}

