#include "gmock/gmock.h"

#define private public

#include "loc.h"
#include "move_history.h"
#include "bdebug.h"

class MoveHistoryFixture : public testing::Test {
public:
	MoveHistoryFixture()
	{
	}

	MoveHistory mh;
};

////////////////////////////////////////////////////////
// Set/Get tests
////////////////////////////////////////////////////////

TEST_F(MoveHistoryFixture, MakeMoveInCentre) {
	Loc centre(9,9);
	mh.saveMove(centre, P1, 0);

#if 0
	Colour c = br.getOcc(centre);
	EXPECT_EQ(P1, c);

	const PriorityLevel &pl = ps.getPriorityLevel(P1, Line1);
	EXPECT_EQ(32, pl.getNumCands());
#endif
}
