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
		buildLineLookupTable();
		buildSpanTable(19);
	}

    PositionStats ps;
    BoardReps br;
	Loc locBuffer[MAX_CANDS];
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

#if 0
TEST_F(BoardRepsFixture, GetAndSet57) {
	Loc l(0,8);
	std::cout << "BJKLBJLBJKLJBJKLBJKLBJKL    " << l._value << std::endl;
	br.setOcc(l, P2);

	Colour c = br.getOcc(l);
	EXPECT_EQ(P2, c);
}

OK
	Loc l(0,0); // 0
	Loc l(0,1); // 32
	Loc l(1,1); // 33
	Loc l(1,2); // 65
	Loc l(0,2); // 64
	Loc l(0,3);
	Loc l(15,15); //495
Fails
	Loc l(2,2); // 66
	Loc l(0,8); // 256
	Loc l(14,15); // 494
	Loc l(14,14); // 462
	Loc l(15,14); // 463

#endif

#if 0
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
#endif

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

TEST_F(BoardRepsFixture, CheckLine3s) {
	buildSpanTable(19);
	br.setOcc(Loc(0,0), P1);
	br.setOcc(Loc(1,0), P1);
	br.setOcc(Loc(2,0), P1);

	const PriorityLevel &pl = ps.getPriorityLevel(P1, Line3);

	EXPECT_EQ(2, pl.getNumCands());
}

TEST_F(BoardRepsFixture, CheckThreatIsAlsoTwo) {
	buildSpanTable(19);
	br.setOcc(Loc(1,0), P2);
	br.setOcc(Loc(2,0), P2);

	// Check threat count
	const PriorityLevel &pl = ps.getPriorityLevel(P1, Threat);
	EXPECT_EQ(2, pl.getNumCands());

	// Check threat candidates
	int numCands = pl.getCands(locBuffer, 5);
	EXPECT_EQ(2, numCands); // as above
	EXPECT_EQ(Loc(3,0), locBuffer[0]);
	EXPECT_EQ(Loc(0,0), locBuffer[1]);
	
	// Check 2->3 length cand. count
	const PriorityLevel &pl2 = ps.getPriorityLevel(P2, Line2);
	EXPECT_EQ(3+1, pl2.getNumCands()); // Two of the candidates are repeated

	// Check 2->3 length candidates
	numCands = pl2.getCands(locBuffer, 5);
	EXPECT_EQ(3+1, numCands); // Count should be the same
	EXPECT_EQ(Loc(5,0), locBuffer[0]);
	EXPECT_EQ(Loc(4,0), locBuffer[1]);
	EXPECT_EQ(Loc(3,0), locBuffer[2]);
	EXPECT_EQ(Loc(0,0), locBuffer[3]);
}

TEST_F(BoardRepsFixture, CheckFarEdgeThreat) {
	buildSpanTable(19);
	br.setOcc(Loc(16,0), P1);
	br.setOcc(Loc(17,0), P1);

	// Check threat count
	const PriorityLevel &pl = ps.getPriorityLevel(P2, Threat);
	EXPECT_EQ(2, pl.getNumCands());

	// Check threat candidates
	int numCands = pl.getCands(locBuffer, 5);
	EXPECT_EQ(2, numCands); // as above
	EXPECT_EQ(Loc(18,0), locBuffer[0]);
	EXPECT_EQ(Loc(15,0), locBuffer[1]);
}
