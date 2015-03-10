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
	//Loc centre(9,9);
	//g.makeMove(centre);
	//Colour c = br.getOcc(Loc(0,0));
	//EXPECT_EQ(EMPTY, c);
}

