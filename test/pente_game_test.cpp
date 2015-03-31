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
		g.setColour(P1);
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

TEST_F(PenteGameFixture, DontCaptureP1) {
	g.makeMove(Loc(3,2), P1);
	g.makeMove(Loc(3,3), P2);
	g.makeMove(Loc(3,1), P1);
	g.makeMove(Loc(3,0), P1);

	CapCount cc = ps.getCaptured(P1);
	EXPECT_EQ(0, cc);

	Colour c1 = br.getOcc(Loc(3,2));
	EXPECT_EQ(P1, c1);
	Colour c2 = br.getOcc(Loc(3,1));
	EXPECT_EQ(P1, c2);
}

TEST_F(PenteGameFixture, DontCaptureP2) {
	g.makeMove(Loc(3,2), P2);
	g.makeMove(Loc(3,3), P1);
	g.makeMove(Loc(3,1), P2);
	g.makeMove(Loc(3,0), P2);

	CapCount cc = ps.getCaptured(P2);
	EXPECT_EQ(0, cc);

	Colour c1 = br.getOcc(Loc(3,2));
	EXPECT_EQ(P2, c1);
	Colour c2 = br.getOcc(Loc(3,1));
	EXPECT_EQ(P2, c2);
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

TEST_F(PenteGameFixture, UndoDoubleCaptureAgain) {
	g.makeMove(Loc(18,7), P1); // Irrel

	g.makeMove(Loc(1,16), P2);
	g.makeMove(Loc(0,15), P1);
	g.makeMove(Loc(2,17), P2);

	g.makeMove(Loc(8,7), P1); // Irrel

	g.makeMove(Loc(1,18), P2);
	g.makeMove(Loc(0,18), P1);
	g.makeMove(Loc(2,18), P2);

	g.makeMove(Loc(3,18), P1);

	CapCount cc = ps.getCaptured(P1);
	EXPECT_EQ(4, cc);

	g.undoLastMove();

	Colour c1 = br.getOcc(Loc(3,18));
	EXPECT_EQ(EMPTY, c1);

	Colour c2 = br.getOcc(Loc(1,16));
	EXPECT_EQ(P2, c2);
	Colour c3 = br.getOcc(Loc(2,17));
	EXPECT_EQ(P2, c3);

	Colour c5 = br.getOcc(Loc(1,18));
	EXPECT_EQ(P2, c2);
	Colour c6 = br.getOcc(Loc(2,18));
	EXPECT_EQ(P2, c3);

	const PriorityLevel &pl = ps.getPriorityLevel(P1, Take);
	EXPECT_EQ(1, pl.getNumCands()); // Only one candidate move, takes 2 pairs

	CapCount cc2 = ps.getCaptured(P1);
	EXPECT_EQ(0, cc2);
}

////////////////////////////////////////////////////////
// Suggester tests
////////////////////////////////////////////////////////

TEST_F(PenteGameFixture, SuggestTake) {
	g.makeMove(Loc(9,9), P1);
	g.makeMove(Loc(9,8), P2);
	g.makeMove(Loc(9,10), P1);
	g.setColour(P2);

	Loc loc = g.makeNextMove();
	EXPECT_EQ(Loc(9,11), loc);

	CapCount cc = ps.getCaptured(P2);
	EXPECT_EQ(2, cc);
}

TEST_F(PenteGameFixture, SuggestAndUndoTwoMoves) {
	g.makeMove(Loc(9,9), P1);
	g.makeMove(Loc(9,8), P2);
	g.makeMove(Loc(9,10), P1);

	Loc loc1 = g.makeNextMove();
	EXPECT_EQ(Loc(9,11), loc1);
	EXPECT_EQ(1, g.getCurrDepth());

	Loc loc2 = g.makeNextMove(); // Should block the Line2 after the capture.
	EXPECT_EQ(2, g.getCurrDepth());

	g.undoLastMove();
	EXPECT_EQ(1, g.getCurrDepth());

	g.undoLastMove();
	EXPECT_EQ(0, g.getCurrDepth());
}

TEST_F(PenteGameFixture, SuggestBothThreatPoints) {
	g.makeMove(Loc(9,9), P1);
	g.makeMove(Loc(7,8), P2);
	g.makeMove(Loc(9,10), P1);

	Loc loc1 = g.makeNextMove();
	EXPECT_EQ(Loc(9,11), loc1);
	g.undoLastMove();

	Loc loc2 = g.makeNextMove(); // Should block the Line2 after the capture.
	EXPECT_EQ(Loc(9,8), loc2);
}

TEST_F(PenteGameFixture, SuggestSWCornerBlock) {
	g.makeMove(Loc(1,1), P1);
	g.makeMove(Loc(7,8), P2);
	g.makeMove(Loc(2,2), P1);
	g.makeMove(Loc(9,11), P2);
	g.makeMove(Loc(3,3), P1);
	g.makeMove(Loc(5,5), P2);
	g.makeMove(Loc(4,4), P1);

    bool oneMove = g.isOnlyOneMove();
	EXPECT_EQ(true, oneMove);

	Loc loc1 = g.makeNextMove();
	EXPECT_EQ(Loc(0,0), loc1);
}

TEST_F(PenteGameFixture, OnlyOneMove) {
	g.makeMove(Loc(9,9), P1);
	g.makeMove(Loc(9,7), P2);
	g.makeMove(Loc(9,8), P1);
	g.makeMove(Loc(9,10), P1);
	g.makeMove(Loc(9,11), P1);

    bool oneMove = g.isOnlyOneMove();
	EXPECT_EQ(true, oneMove);
}

TEST_F(PenteGameFixture, WinP1InOneMove) {
	g.makeMove(Loc(0,0), P1);
	g.makeMove(Loc(0,1), P1);
	g.makeMove(Loc(0,2), P1);
	g.makeMove(Loc(0,3), P1);

	g.makeMove(Loc(2,0), P2);
	g.makeMove(Loc(2,1), P2);
	g.makeMove(Loc(2,2), P2);
	g.makeMove(Loc(2,3), P2);

	g.setColour(P1);
    bool oneMove = g.isOnlyOneMove();
	EXPECT_EQ(true, oneMove);

	Loc loc1 = g.makeNextMove();
	EXPECT_EQ(Loc(0,4), loc1);
}

TEST_F(PenteGameFixture, WinP2InOneMove) {
	g.makeMove(Loc(0,0), P1);
	g.makeMove(Loc(0,1), P1);
	g.makeMove(Loc(0,2), P1);
	g.makeMove(Loc(0,3), P1);

	g.makeMove(Loc(2,0), P2);
	g.makeMove(Loc(2,1), P2);
	g.makeMove(Loc(2,2), P2);
	g.makeMove(Loc(2,3), P2);

	g.setColour(P2);
    bool oneMove = g.isOnlyOneMove();
	EXPECT_EQ(true, oneMove);

	Loc loc1 = g.makeNextMove();
	EXPECT_EQ(Loc(2,4), loc1);
}

TEST_F(PenteGameFixture, NotOnlyOneMove) {
	g.makeMove(Loc(9,9), P1);
	g.makeMove(Loc(9,8), P2);
	g.makeMove(Loc(9,10), P1);

    bool oneMove = g.isOnlyOneMove();
	EXPECT_EQ(false, oneMove);
}

TEST_F(PenteGameFixture, UtilityIsConnected) {
	g.makeMove(Loc(9,9), P1);
	UtilityValue uv1 = g.getUtility();

	g.makeMove(Loc(9,7), P1);
	g.makeMove(Loc(0,0), P2);
	UtilityValue uv2 = g.getUtility();

	EXPECT_GT(uv2, uv1);
}

#if 0
// Disabled for now, not showing a speedup in system tests.
TEST_F(PenteGameFixture, SaveAndUseTranspositionTable) {
	g._currDepth = 6; // Hack
	g.makeMove(Loc(9,9), P1);
	g.makeMove(Loc(0,0), P2);
	g.makeMove(Loc(9,8), P1);
	g.makeMove(Loc(8,4), P2);
	g.makeMove(Loc(7,9), P1);
	g.makeMove(Loc(2,0), P2);
	UtilityValue uv1 = g.getUtility();
    g.undoLastMove();
    g.undoLastMove();
    g.undoLastMove();
    g.undoLastMove();
    g.undoLastMove();
    g.undoLastMove();

	g.makeMove(Loc(9,9), P1);
	g.makeMove(Loc(2,0), P2);
	g.makeMove(Loc(9,8), P1);
	g.makeMove(Loc(8,4), P2);
	g.makeMove(Loc(7,9), P1);
	g.makeMove(Loc(0,0), P2);
	EXPECT_EQ(true, g.isInTT());

	g.clearTT();
	EXPECT_EQ(false, g.isInTT());
}
#endif

////////////////////////////////////////////////////////
// Different Rules
////////////////////////////////////////////////////////

TEST_F(PenteGameFixture, CannotPlayOutsideBoard) {
	g.setBoardSize(9);

	bool ok;
	ok = g.isLegalMove(Loc(8,8));
	EXPECT_EQ(true, ok);
	ok = g.isLegalMove(Loc(9,8));
	EXPECT_EQ(false, ok);
	ok = g.isLegalMove(Loc(8,9));
	EXPECT_EQ(false, ok);
}

TEST_F(PenteGameFixture, MustPlayCentre) {
	g.setRules("t");
	g.setBoardSize(9);

	bool ok;
	ok = g.isLegalMove(Loc(4,5));
	EXPECT_EQ(false, ok);
	ok = g.isLegalMove(Loc(5,4));
	EXPECT_EQ(false, ok);
	ok = g.isLegalMove(Loc(4,4));
	EXPECT_EQ(true, ok);
}

TEST_F(PenteGameFixture, DoNotNeedToPlayCentreInStandard) {
	g.setRules("s");
	g.setBoardSize(13);

	bool ok;
	ok = g.isLegalMove(Loc(4,5));
	EXPECT_EQ(true, ok);
	ok = g.isLegalMove(Loc(5,4));
	EXPECT_EQ(true, ok);
	ok = g.isLegalMove(Loc(6,6));
	EXPECT_EQ(true, ok);
}

TEST_F(PenteGameFixture, DoNotNeedToPlayCentreIn5_I_A_R) {
	g.setRules("5");
	g.setBoardSize(13);

	bool ok;
	ok = g.isLegalMove(Loc(4,5));
	EXPECT_EQ(true, ok);
	ok = g.isLegalMove(Loc(5,4));
	EXPECT_EQ(true, ok);
	ok = g.isLegalMove(Loc(6,6));
	EXPECT_EQ(true, ok);
}

TEST_F(PenteGameFixture, TournamentRule) {
	g.setRules("t");
	g.setBoardSize(19);
	g.makeMove(Loc(9,9), P1);
	g.makeMove(Loc(8,8), P2);

	bool ok;
	ok = g.isLegalMove(Loc(8,9));
	EXPECT_EQ(false, ok);
	ok = g.isLegalMove(Loc(6,9));
	EXPECT_EQ(true, ok);
	ok = g.isLegalMove(Loc(9,6));
	EXPECT_EQ(true, ok);
	ok = g.isLegalMove(Loc(12,9));
	EXPECT_EQ(true, ok);
	ok = g.isLegalMove(Loc(9,12));
	EXPECT_EQ(true, ok);

	ok = g.isLegalMove(Loc(7,7));
	EXPECT_EQ(false, ok);
	ok = g.isLegalMove(Loc(11,11));
	EXPECT_EQ(false, ok);
}

TEST_F(PenteGameFixture, NoTournamentRuleInStandard) {
	g.setRules("s");
	g.setBoardSize(19);
	g.makeMove(Loc(9,9), P1);
	g.makeMove(Loc(8,8), P2);

	bool ok;
	ok = g.isLegalMove(Loc(8,9));
	EXPECT_EQ(true, ok);
	ok = g.isLegalMove(Loc(6,9));
	EXPECT_EQ(true, ok);
	ok = g.isLegalMove(Loc(9,6));
	EXPECT_EQ(true, ok);
	ok = g.isLegalMove(Loc(12,9));
	EXPECT_EQ(true, ok);
	ok = g.isLegalMove(Loc(9,12));
	EXPECT_EQ(true, ok);

	ok = g.isLegalMove(Loc(7,7));
	EXPECT_EQ(true, ok);
	ok = g.isLegalMove(Loc(11,11));
	EXPECT_EQ(true, ok);
}

TEST_F(PenteGameFixture, NoTournamentRuleInF_I_A_R) {
	g.setRules("5");
	g.setBoardSize(19);
	g.makeMove(Loc(9,9), P1);
	g.makeMove(Loc(8,8), P2);

	bool ok;
	ok = g.isLegalMove(Loc(8,9));
	EXPECT_EQ(true, ok);
	ok = g.isLegalMove(Loc(6,9));
	EXPECT_EQ(true, ok);
	ok = g.isLegalMove(Loc(9,6));
	EXPECT_EQ(true, ok);
	ok = g.isLegalMove(Loc(12,9));
	EXPECT_EQ(true, ok);
	ok = g.isLegalMove(Loc(9,12));
	EXPECT_EQ(true, ok);

	ok = g.isLegalMove(Loc(7,7));
	EXPECT_EQ(true, ok);
	ok = g.isLegalMove(Loc(11,11));
	EXPECT_EQ(true, ok);
}
