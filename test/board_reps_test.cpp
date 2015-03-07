#include "gmock/gmock.h"

#define private public

#include "loc.h"
#include "position_stats.h"
#include "board_reps.h"
#include "bdebug.h"

class BoardRepsFixture : public testing::Test {
public:
	BoardRepsFixture() : br(ps) {}

    PositionStats ps;
    BoardReps br;
};

TEST_F(BoardRepsFixture, EmptyBoardGetZZEmpty) {
	Colour c = br.getOcc(Loc(0,0));
	EXPECT_EQ(EMPTY, c);
}	

TEST_F(BoardRepsFixture, GetAndSetZZ) {
	br.setOcc(Loc(0,0), P1);

	Colour c = br.getOcc(Loc(0,0));
	EXPECT_EQ(P1, c);
}

TEST_F(BoardRepsFixture, GetAndSet11) {
	br.setOcc(Loc(1,1), P1);

	Colour c = br.getOcc(Loc(1,1));
	EXPECT_EQ(P1, c);
}

TEST_F(BoardRepsFixture, GetAndSet23) {
	br.setOcc(Loc(2,3), P2);

	Colour c = br.getOcc(Loc(2,3));
	EXPECT_EQ(P2, c);
}

TEST_F(BoardRepsFixture, GetAndSetMaxMax) {
	br.setOcc(Loc(18,18), P2);

	Colour c = br.getOcc(Loc(18,18));
	EXPECT_EQ(P2, c);
}

#if 0
// TODO

//////////////////////////////
// Reporting to PositionStats
//////////////////////////////

TEST_F(BoardRepsFixture, CheckLine1s) {
	br.setOcc(Loc(0,0), P1);

	const PriorityLevel &pl = ps.getLengthPriorityLevel(P1, 1);

	EXPECT_EQ(3, pl.getNumCands());
}
#endif
