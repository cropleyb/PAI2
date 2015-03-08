#include "gmock/gmock.h"

#define private public

#include "loc.h"
#include "position_stats.h"
#include "board_reps.h"
#include "bdebug.h"
#include "span_lookup_table.h"
#include "line_lookup_table.h"

class BoardRepsFixture : public testing::Test {
public:
	BoardRepsFixture() : br(19, ps)
	{
		buildAll(); // TODO rename
		buildSpanTable(19);
	}

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

TEST_F(BoardRepsFixture, GetAndSet1515) {
	br.setOcc(Loc(15,15), P2);

	Colour c = br.getOcc(Loc(15,15));
	EXPECT_EQ(P2, c);
}

TEST_F(BoardRepsFixture, GetAndSetMaxMax) {
	br.setOcc(Loc(18,18), P2);

	Colour c = br.getOcc(Loc(18,18));
	EXPECT_EQ(P2, c);
}


//////////////////////////////
// Reporting to PositionStats
//////////////////////////////

TEST_F(BoardRepsFixture, CheckLine1s) {
	buildSpanTable(19);
	br.setOcc(Loc(0,0), P1);

	const PriorityLevel &pl = ps.getPriorityLevel(P1, Line1);

	EXPECT_EQ(12, pl.getNumCands());
}

TEST_F(BoardRepsFixture, CheckLine2s) {
	buildSpanTable(19);
	br.setOcc(Loc(0,0), P1);
	br.setOcc(Loc(1,0), P1);

	const PriorityLevel &pl = ps.getPriorityLevel(P1, Line2);

	EXPECT_EQ(3, pl.getNumCands());
}

#if 0
TEST_F(BoardRepsFixture, CheckLine3s) {
	buildSpanTable(19);
	br.setOcc(Loc(0,0), P1);
	br.setOcc(Loc(1,0), P1);
	br.setOcc(Loc(2,0), P1);

	const PriorityLevel &pl = ps.getPriorityLevel(P1, Line3);

	EXPECT_EQ(2, pl.getNumCands());
}

TEST_F(BoardRepsFixture, CheckThreat) {
	buildSpanTable(19);
	br.setOcc(Loc(1,0), P2);
	br.setOcc(Loc(2,0), P2);

	const PriorityLevel &pl = ps.getPriorityLevel(P1, Threat);

	EXPECT_EQ(2, pl.getNumCands());
}
#endif
