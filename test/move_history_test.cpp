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
	MoveNumber mn = mh.saveMove(centre, P1, 0);
	EXPECT_EQ(1, mn);

	Loc moved = mh.getMoved(0);
	EXPECT_EQ(centre, moved);

	CaptureDirs cap = mh.getCapDirs(0);
	EXPECT_EQ(0, cap);
}

TEST_F(MoveHistoryFixture, MakeTwoMoves) {
	Loc m1(9,9);
	Loc m2(8,8);
	MoveNumber mn1 = mh.saveMove(m1, P1, 13);
	MoveNumber mn2 = mh.saveMove(m2, P2, 24);
	EXPECT_EQ(2, mn2);

	Loc moved1 = mh.getMoved(0);
	EXPECT_EQ(m1, moved1);
	Loc moved2 = mh.getMoved(1);
	EXPECT_EQ(m2, moved2);

	CaptureDirs cap1 = mh.getCapDirs(0);
	EXPECT_EQ(13, cap1);
	CaptureDirs cap2 = mh.getCapDirs(1);
	EXPECT_EQ(24, cap2);
}
