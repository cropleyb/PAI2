#include "gmock/gmock.h"

#define private public

#include "loc.h"
#include "position_stats.h"
#include "pente_game.h"
#include "bdebug.h"

class PenteGameFixture : public testing::Test {
public:
	PenteGameFixture() : ps(g._posStats), br(g._boardReps)
	{
	}

	PenteGame g;
    PositionStats &ps;
    BoardReps &br;
	Loc locBuffer[MAX_CANDS];
};

////////////////////////////////////////////////////////
// Set/Get tests
////////////////////////////////////////////////////////

TEST_F(PenteGameFixture, MakeMoveInCentre) {
	Loc centre(9,9);
	g.makeMove(centre, P1);

	Colour c = br.getOcc(centre);
	EXPECT_EQ(P1, c);

	const PriorityLevel &pl = ps.getPriorityLevel(P1, Line1);
	EXPECT_EQ(32, pl.getNumCands());
}

TEST_F(PenteGameFixture, MakeThreatInCentre) {
	g.makeMove(Loc(9,9), P1);
	g.makeMove(Loc(3,5), P2);
	g.makeMove(Loc(9,8), P1);

	const PriorityLevel &pl = ps.getPriorityLevel(P2, Threat);
	EXPECT_EQ(2, pl.getNumCands());
}

TEST_F(PenteGameFixture, MakeTakeInCentre) {
	g.makeMove(Loc(9,9), P1);
	g.makeMove(Loc(7,7), P2);
	g.makeMove(Loc(8,8), P1);

	const PriorityLevel &pl = ps.getPriorityLevel(P2, Take);
	EXPECT_EQ(1, pl.getNumCands());
}

////////////////////////////////////////////////////////
// Capture tests
////////////////////////////////////////////////////////

TEST_F(PenteGameFixture, CaptureInCentre) {
	g.makeMove(Loc(9,9), P1);
	g.makeMove(Loc(7,7), P2);
	g.makeMove(Loc(8,8), P1);
	g.makeMove(Loc(10,10), P2);

	CapCount cc = ps.getCaptured(P2);
	EXPECT_EQ(2, cc);

	Colour c1 = br.getOcc(Loc(9,9));
	EXPECT_EQ(EMPTY, c1);
	Colour c2 = br.getOcc(Loc(8,8));
	EXPECT_EQ(EMPTY, c2);
}

TEST_F(PenteGameFixture, CaptureByP1) {
	g.makeMove(Loc(9,9), P2);
	g.makeMove(Loc(7,7), P1);
	g.makeMove(Loc(8,8), P2);
	g.makeMove(Loc(10,10), P1);

	CapCount cc = ps.getCaptured(P1);
	EXPECT_EQ(2, cc);

	Colour c1 = br.getOcc(Loc(9,9));
	EXPECT_EQ(EMPTY, c1);
	Colour c2 = br.getOcc(Loc(8,8));
	EXPECT_EQ(EMPTY, c2);
}

TEST_F(PenteGameFixture, CaptureToLeft) {
	g.makeMove(Loc(9,9), P2);
	g.makeMove(Loc(10,10), P1);
	g.makeMove(Loc(8,8), P2);
	g.makeMove(Loc(7,7), P1);

	CapCount cc = ps.getCaptured(P1);
	EXPECT_EQ(2, cc);

	Colour c1 = br.getOcc(Loc(9,9));
	EXPECT_EQ(EMPTY, c1);
	Colour c2 = br.getOcc(Loc(8,8));
	EXPECT_EQ(EMPTY, c2);
}

TEST_F(PenteGameFixture, CaptureOnLowerEdge) {
	g.makeMove(Loc(3,2), P2);
	g.makeMove(Loc(3,3), P1);
	g.makeMove(Loc(3,1), P2);
	g.makeMove(Loc(3,0), P1);

	CapCount cc = ps.getCaptured(P1);
	EXPECT_EQ(2, cc);

	Colour c1 = br.getOcc(Loc(3,2));
	EXPECT_EQ(EMPTY, c1);
	Colour c2 = br.getOcc(Loc(3,1));
	EXPECT_EQ(EMPTY, c2);
}

TEST_F(PenteGameFixture, CaptureInBigCorner) {
	g.makeMove(Loc(16,16), P2);
	g.makeMove(Loc(15,15), P1);
	g.makeMove(Loc(17,17), P2);
	g.makeMove(Loc(18,18), P1);

	CapCount cc = ps.getCaptured(P1);
	EXPECT_EQ(2, cc);

	Colour c1 = br.getOcc(Loc(16,16));
	EXPECT_EQ(EMPTY, c1);
	Colour c2 = br.getOcc(Loc(17,17));
	EXPECT_EQ(EMPTY, c2);
}

TEST_F(PenteGameFixture, CaptureTwoEdges) {
	g.makeMove(Loc(16,17), P1);
	g.makeMove(Loc(15,18), P2);
	g.makeMove(Loc(17,16), P1);
	g.makeMove(Loc(18,15), P2);

	CapCount cc = ps.getCaptured(P2);
	EXPECT_EQ(2, cc);

	Colour c1 = br.getOcc(Loc(16,17));
	EXPECT_EQ(EMPTY, c1);
	Colour c2 = br.getOcc(Loc(17,16));
	EXPECT_EQ(EMPTY, c2);
}

TEST_F(PenteGameFixture, CaptureTwoPairs) {
	g.makeMove(Loc(16,17), P1);
	g.makeMove(Loc(15,18), P2);
	g.makeMove(Loc(17,16), P1);

	g.makeMove(Loc(18,17), P1);
	g.makeMove(Loc(18,18), P2);
	g.makeMove(Loc(18,16), P1);

	g.makeMove(Loc(18,15), P2);

	CapCount cc = ps.getCaptured(P2);
	EXPECT_EQ(4, cc);

	Colour c1 = br.getOcc(Loc(16,17));
	EXPECT_EQ(EMPTY, c1);
	Colour c2 = br.getOcc(Loc(17,16));
	EXPECT_EQ(EMPTY, c2);
	Colour c3 = br.getOcc(Loc(18,17));
	EXPECT_EQ(EMPTY, c3);
	Colour c4 = br.getOcc(Loc(18,16));
	EXPECT_EQ(EMPTY, c4);
}

////////////////////////////////////////////////////////
// Undo tests
////////////////////////////////////////////////////////

TEST_F(PenteGameFixture, UndoMoveInCentre) {
	Loc centre(9,9);
	g.makeMove(centre, P1);
	g.undoLastMove();

	Colour c = br.getOcc(centre);
	EXPECT_EQ(EMPTY, c);

	const PriorityLevel &pl = ps.getPriorityLevel(P1, Line1);
	EXPECT_EQ(0, pl.getNumCands());
}

TEST_F(PenteGameFixture, UndoTwoMoves) {
	Loc m1(9,9);
	g.makeMove(m1, P1);
	Loc m2(8,8);
	g.makeMove(m2, P2);

	g.undoLastMove();
	g.undoLastMove();

	Colour c1 = br.getOcc(m1);
	EXPECT_EQ(EMPTY, c1);
	Colour c2 = br.getOcc(m2);
	EXPECT_EQ(EMPTY, c2);

	const PriorityLevel &pl = ps.getPriorityLevel(P1, Line1);
	EXPECT_EQ(0, pl.getNumCands());
}

TEST_F(PenteGameFixture, UndoCapture) {
	g.makeMove(Loc(16,17), P1);
	g.makeMove(Loc(15,18), P2);
	g.makeMove(Loc(17,16), P1);
	g.makeMove(Loc(18,15), P2);

	CapCount cc = ps.getCaptured(P2);
	EXPECT_EQ(2, cc);

	g.undoLastMove();

	Colour c1 = br.getOcc(Loc(18,15));
	EXPECT_EQ(EMPTY, c1);
	Colour c2 = br.getOcc(Loc(17,16));
	EXPECT_EQ(P1, c2);
	Colour c3 = br.getOcc(Loc(16,17));
	EXPECT_EQ(P1, c3);

	const PriorityLevel &pl = ps.getPriorityLevel(P2, Take);
	EXPECT_EQ(1, pl.getNumCands());

	CapCount cc2 = ps.getCaptured(P2);
	EXPECT_EQ(0, cc2);
}

TEST_F(PenteGameFixture, UndoDoubleCapture) {
	g.makeMove(Loc(16,17), P1);
	g.makeMove(Loc(15,18), P2);
	g.makeMove(Loc(17,16), P1);

	g.makeMove(Loc(8,7), P2); // Irrel

	g.makeMove(Loc(18,17), P1);
	g.makeMove(Loc(18,18), P2);
	g.makeMove(Loc(18,16), P1);

	g.makeMove(Loc(18,15), P2);

	CapCount cc = ps.getCaptured(P2);
	EXPECT_EQ(4, cc);

	g.undoLastMove();

	Colour c1 = br.getOcc(Loc(18,15));
	EXPECT_EQ(EMPTY, c1);

	Colour c2 = br.getOcc(Loc(17,16));
	EXPECT_EQ(P1, c2);
	Colour c3 = br.getOcc(Loc(16,17));
	EXPECT_EQ(P1, c3);

	Colour c5 = br.getOcc(Loc(18,16));
	EXPECT_EQ(P1, c2);
	Colour c6 = br.getOcc(Loc(18,17));
	EXPECT_EQ(P1, c3);

	const PriorityLevel &pl = ps.getPriorityLevel(P2, Take);
	EXPECT_EQ(1, pl.getNumCands()); // Only one candidate move, takes 2 pairs

	CapCount cc2 = ps.getCaptured(P2);
	EXPECT_EQ(0, cc2);
}
