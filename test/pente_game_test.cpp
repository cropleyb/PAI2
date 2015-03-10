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

TEST_F(PenteGameFixture, CaptureInCentre) {
	g.makeMove(Loc(9,9), P1);
	g.makeMove(Loc(7,7), P2);
	g.makeMove(Loc(8,8), P1);
	g.makeMove(Loc(10,10), P2);

	CapCount cc = ps.getCaptured(P2);
	EXPECT_EQ(2, cc);
}

