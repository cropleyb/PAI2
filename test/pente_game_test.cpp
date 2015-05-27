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

TEST_F(PenteGameFixture, UndoThreatAndLine2) {
	g.makeMove(Loc(9,9), P1);
	g.makeMove(Loc(7,8), P2);
	g.makeMove(Loc(9,10), P1);

	int p2threats = g._posStats.getPriorityLevel(P2, Threat).getNumCands();
	EXPECT_EQ(2, p2threats);
	int p1line2s = g._posStats.getPriorityLevel(P1, Line2).getNumCands();
	EXPECT_EQ(6, p1line2s);

	g.undoLastMove();

	p2threats = g._posStats.getPriorityLevel(P2, Threat).getNumCands();
	EXPECT_EQ(0, p2threats);
	p1line2s = g._posStats.getPriorityLevel(P1, Line2).getNumCands();
	EXPECT_EQ(0, p1line2s);
}

TEST_F(PenteGameFixture, PlayThreat) {
	g.makeMove(Loc(9,7), P1);
	g.makeMove(Loc(7,4), P2);
	g.makeMove(Loc(9,6), P1);

	int p2threats = g._posStats.getPriorityLevel(P2, Threat).getNumCands();
	EXPECT_EQ(2, p2threats);
	int p1line2s = g._posStats.getPriorityLevel(P1, Line2).getNumCands();
	EXPECT_EQ(6, p1line2s);

	g.makeMove(Loc(9,5), P2);

	p2threats = g._posStats.getPriorityLevel(P2, Threat).getNumCands();
	EXPECT_EQ(0, p2threats);
	p1line2s = g._posStats.getPriorityLevel(P1, Line2).getNumCands();
	EXPECT_EQ(3, p1line2s);

	g.undoLastMove();

	p2threats = g._posStats.getPriorityLevel(P2, Threat).getNumCands();
	EXPECT_EQ(2, p2threats);
	p1line2s = g._posStats.getPriorityLevel(P1, Line2).getNumCands();
	EXPECT_EQ(6, p1line2s);
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
	g.setRules('t');
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
	g.setRules('s');
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
	g.setRules('5');
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
	g.setRules('t');
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
	g.setRules('s');
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
	g.setRules('5');
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

TEST_F(PenteGameFixture, NoWinByCapturesIn5) {
	g.setRules('5');
	g.setBoardSize(19);

	g.makeMove(Loc(2,1), P1);
	g.makeMove(Loc(1,1), P2);
	g.makeMove(Loc(3,1), P1);
	g.makeMove(Loc(4,1), P2); // Cap 1

	g.makeMove(Loc(2,3), P1);
	g.makeMove(Loc(1,3), P2);
	g.makeMove(Loc(3,3), P1);
	g.makeMove(Loc(4,3), P2); // Cap 2

	g.makeMove(Loc(2,5), P1);
	g.makeMove(Loc(1,5), P2);
	g.makeMove(Loc(3,5), P1);
	g.makeMove(Loc(4,5), P2); // Cap 3

	g.makeMove(Loc(2,7), P1);
	g.makeMove(Loc(1,7), P2);
	g.makeMove(Loc(3,7), P1);
	g.makeMove(Loc(4,7), P2); // Cap 4

	Colour winnerA = g.getWonBy();
	EXPECT_EQ(EMPTY, winnerA);

	g.makeMove(Loc(2,9), P1);
	g.makeMove(Loc(1,9), P2);
	g.makeMove(Loc(3,9), P1);
	g.makeMove(Loc(4,9), P2); // Cap 5

	Colour winner = g.getWonBy();
	EXPECT_EQ(EMPTY, winner);
}

TEST_F(PenteGameFixture, MakeFirstMoveInTrnmt) {
	g.setRules('t');
	g.setBoardSize(19);

	Loc loc = g.makeNextMove();
	EXPECT_EQ(Loc(9,9), loc);
}

TEST_F(PenteGameFixture, MakeFirstMoveInStd) {
	g.setRules('s');
	g.setBoardSize(19);

	Loc loc = g.makeNextMove();
	EXPECT_EQ(Loc(9,9), loc);
}

TEST_F(PenteGameFixture, MakeFirstMoveIn_F_I_A_R) {
	g.setRules('5');
	g.setBoardSize(19);

	Loc loc = g.makeNextMove();
	EXPECT_EQ(Loc(9,9), loc);
}

TEST_F(PenteGameFixture, UndoCaptureUpdatesPosStats) {
	g.setRules('s');
	g.setBoardSize(19);
	g.makeMove(Loc(2,9), P1);
	g.makeMove(Loc(1,9), P2);
	g.makeMove(Loc(3,9), P1);
	g.makeMove(Loc(4,9), P2);

	const PriorityLevel &ourTwosBefore = g._posStats.getPriorityLevel(P2, Line2);
	EXPECT_EQ(4, ourTwosBefore.getNumCands());

	g.undoLastMove();

	const PriorityLevel &ourTwosAfter = g._posStats.getPriorityLevel(P2, Line2);
	EXPECT_EQ(0, ourTwosAfter.getNumCands());
}

/////////////////////////////////////////////////////////
// Restart game tests
/////////////////////////////////////////////////////////

TEST_F(PenteGameFixture, RestartTakesUsAllTheWayBack) {
	g.setRules('s');
	g.setBoardSize(19);
	g.makeMove(Loc(9,9), P1);
	g.makeMove(Loc(8,8), P2);
	g.makeMove(Loc(6,9), P1);
	g.makeMove(Loc(10,8), P2);
	g.restartGame();

	const PriorityLevel &p1Line1s = g._posStats.getPriorityLevel(P1, Line1);
	EXPECT_EQ(0, p1Line1s.getNumCands());

	Colour centre = g.getOcc(Loc(9,9));
	EXPECT_EQ(EMPTY, centre);
}

/////////////////////////////////////////////////////////
// VCT tests
/////////////////////////////////////////////////////////

#if 0
TEST_F(PenteGameFixture, VCT) {
	g.setRules('s');
	g.setBoardSize(19);
	g.setMaxDepth(1);
	g.makeMove(Loc(9,9), P1);
	g.makeMove(Loc(8,8), P2);
	g.makeMove(Loc(6,9), P1);
	g.makeMove(Loc(10,8), P2);

	const PriorityLevel &p1Line1s = g._posStats.getPriorityLevel(P1, Line1);
	EXPECT_EQ(0, p1Line1s.getNumCands());

	Colour centre = g.getOcc(Loc(9,9));
	EXPECT_EQ(EMPTY, centre);
}
#endif

/////////////////////////////////////////////////////////
// Take targeting
/////////////////////////////////////////////////////////

TEST_F(PenteGameFixture, TakeFourWithFourFirst) {
	g.setRules('s');
	g.setBoardSize(19);
	g._posStats._takeTargeting = true;

	// P1 gets a (double) line of 4 from (9,9) to (6,9)
	// P2 can take (9,9) with (11,11)
	g.makeMove(Loc(9,9), P1);
	g.makeMove(Loc(8,8), P2);
	g.makeMove(Loc(8,9), P1);
	g.makeMove(Loc(8,6), P2);
	g.makeMove(Loc(7,9), P1);
	g.makeMove(Loc(8,3), P2);
	g.makeMove(Loc(6,9), P1);
	g.makeMove(Loc(8,1), P2);
	g.makeMove(Loc(10,10), P1);

	const PriorityLevel &pl = g._posStats.getPriorityLevel(P2, FourTake);
	EXPECT_EQ(1, pl.getNumCands());

	// take it
	g.makeMove(Loc(11,11), P2);
	EXPECT_EQ(0, pl.getNumCands());

	// undo
	g.undoLastMove();
	EXPECT_EQ(1, pl.getNumCands());

	// and again to the point before the FourTake should have been created.
	g.undoLastMove();
	EXPECT_EQ(0, pl.getNumCands());
}


//#include <iostream>
//using namespace std;

// b pente_game.cpp:154
// run --gtest_filter="*TakeFourWithT*" 
TEST_F(PenteGameFixture, TakeFourWithTakeFirst) {
	g.setRules('s');
	g.setBoardSize(19);
	g._posStats._takeTargeting = true;

	// Starting with P2 is dodgy, but currently works...
	g.makeMove(Loc(9,9), P2);
	g.makeMove(Loc(8,8), P1);
	g.makeMove(Loc(9,8), P2);
	g.makeMove(Loc(6,8), P1);
	g.makeMove(Loc(9,7), P2);
	g.makeMove(Loc(3,8), P1);
	g.makeMove(Loc(10,10), P2);
	g.makeMove(Loc(1,8), P1);

	g.makeMove(Loc(9,6), P2);

	EXPECT_EQ(2, g._posStats.getPriorityLevel(P2, Line4).getNumCands());

	const PriorityLevel &pl = g._posStats.getPriorityLevel(P1, FourTake);
	EXPECT_EQ(1, pl.getNumCands());

	// take it
	g.makeMove(Loc(11,11), P1);
	EXPECT_EQ(0, pl.getNumCands());
	EXPECT_EQ(0, g._posStats.getPriorityLevel(P2, Line4).getNumCands());

	// undo
	g.undoLastMove();
	EXPECT_EQ(2, g._posStats.getPriorityLevel(P2, Line4).getNumCands());
	EXPECT_EQ(1, pl.getNumCands());

	// and again to the point before the FourTake should have been created.
	g.undoLastMove();
	EXPECT_EQ(0, pl.getNumCands());
}

/////////////////////////////////////////////////
// BlockedFourTakes
/////////////////////////////////////////////////

TEST_F(PenteGameFixture, TakeBlockedFourWithBlockFirst) {
	g.setRules('s');
	g.setBoardSize(19);
	g._posStats._takeTargeting = true;
	const PriorityLevel &pl = g._posStats.getPriorityLevel(P1, Blocked4Take);

	// P1 gets a blocked four, then a P1 take is created against the block.
	// XOO
	//  X
	//  X
	//  X
	//  X
	g.makeMove(Loc(9,9), P1);
	g.makeMove(Loc(8,8), P2);
	g.makeMove(Loc(9,8), P1);
	g.makeMove(Loc(6,8), P2);
	g.makeMove(Loc(9,7), P1);
	g.makeMove(Loc(3,8), P2);
	cout << "TEST MAKE THE 4" << endl;
	g.makeMove(Loc(9,6), P1); // That's 4 in a row
	cout << "TEST MAKE THE BLOCK" << endl;
	g.makeMove(Loc(9,10), P2); // The block
	g.makeMove(Loc(8,10), P1);
	EXPECT_EQ(0, pl.getNumCands());

	cout << "TEST MAKE THE TAKE" << endl;
	g.makeMove(Loc(10,10), P2); // Create the Take
	EXPECT_EQ(1, pl.getNumCands());

	// take it
	g.makeMove(Loc(11,10), P2);
	EXPECT_EQ(0, pl.getNumCands());

	// undo
	g.undoLastMove();
	EXPECT_EQ(1, pl.getNumCands());

	// and again to the point before the Blocked4FourTake should have been created.
	g.undoLastMove();
	EXPECT_EQ(0, pl.getNumCands());
}

TEST_F(PenteGameFixture, TakeBlockedFourWithTakeFirst) {
	g.setRules('s');
	g.setBoardSize(19);
	g._posStats._takeTargeting = true;
	const PriorityLevel &pl = g._posStats.getPriorityLevel(P1, Blocked4Take);

	// P1 gets a take, then a blocked 4 is created against it.
	// XOO
	//  X
	//  X
	//  X
	//  X
	g.makeMove(Loc(9,9), P1);
	g.makeMove(Loc(8,8), P2);
	g.makeMove(Loc(9,8), P1);
	g.makeMove(Loc(6,8), P2);
	g.makeMove(Loc(8,10), P1);
	cout << "TEST MAKE THE TAKE" << endl;
	g.makeMove(Loc(10,10), P2); // Create the Take
	g.makeMove(Loc(9,7), P1);
	cout << "TEST MAKE THE BLOCK" << endl;
	g.makeMove(Loc(9,10), P2); // The block
	EXPECT_EQ(0, pl.getNumCands());

	cout << "TEST MAKE THE 4" << endl;
	g.makeMove(Loc(9,6), P1); // That's 4 in a row

	EXPECT_EQ(1, pl.getNumCands());

	// take it
	g.makeMove(Loc(11,10), P2);
	EXPECT_EQ(0, pl.getNumCands());

	// undo
	g.undoLastMove();
	EXPECT_EQ(1, pl.getNumCands());

	// and again to the point before the Blocked4Take should have been created.
	g.undoLastMove();
	EXPECT_EQ(0, pl.getNumCands());
}

// TODO: block not at the end tests

/////////////////////////////////////////////////
// TakeTakes
/////////////////////////////////////////////////

TEST_F(PenteGameFixture, TakeTakeWithTargetFirst) {
	g.setRules('s');
	g.setBoardSize(19);
	g._posStats._takeTargeting = true;
	const PriorityLevel &pl = g._posStats.getPriorityLevel(P2, TakeTake);

	// P1 gets a take, then a P2 take is created against the first take's target.
	// XOO
	// X
	// O
	g.makeMove(Loc(9,9), P1);
	g.makeMove(Loc(10,9), P2);
	g.makeMove(Loc(9,8), P1);
	cout << "TEST MAKE THE TARGET TAKE (P2 (9,9))" << endl;
	g.makeMove(Loc(11,9), P2);
	g.makeMove(Loc(3,3), P1); // Irrel.
	EXPECT_EQ(0, pl.getNumCands());

	cout << "TEST MAKE THE TAKER (P2 (9,10))" << endl;
	g.makeMove(Loc(9,7), P2);
	EXPECT_EQ(1, pl.getNumCands());
	g.makeMove(Loc(4,4), P1); // Irrel.

	cout << "TEST MAKE THE CAPTURE" << endl;
	g.makeMove(Loc(9,10), P2); // capture it
	EXPECT_EQ(0, pl.getNumCands());

	// undo
	g.undoLastMove();
	EXPECT_EQ(1, pl.getNumCands());

	// and again to the point before the TakeTake should have been created.
	g.undoLastMove();
	g.undoLastMove();
	EXPECT_EQ(0, pl.getNumCands());
}

