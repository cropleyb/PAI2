#include "gmock/gmock.h"

#define private public

#include "defines.h"
#include "loc.h"
#include "candidate_cache.h"

class CandidateCacheFixture : public testing::Test {
public:
	CandidateCache cc;
	Loc locBuffer[MAX_DEPTH][MAX_CANDS];
};

using ::testing::AtLeast;
using ::testing::Return;
using ::testing::InSequence;

//using std::vector;
//typedef vector<Loc> LocArr;

TEST_F(CandidateCacheFixture, NoMovesYet) {
	Loc l = cc.getNextMove();

	EXPECT_EQ(Loc::INVALID, l);
}

TEST_F(CandidateCacheFixture, GetDepth0Buffer) {
	Loc l = Loc(4,7);
    Loc *locs = cc.getBuffer(0);
	locs[0] = l;
	cc.setDepthMoves(0,1);
	Loc l2 = cc.getNextMove();

	EXPECT_EQ(l, l2);
}

