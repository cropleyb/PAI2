// #include "gmock/gmock.h"

#define private public

#include "move.h"
#include "gmock/gmock.h"

class MoveFixture : public testing::Test {
public:

};

TEST_F(MoveFixture, GetMoveLocationAndCaps) {
	Loc l(4,7);
	Directions capDirections = 3;
	Move m(l, capDirections);
	
	EXPECT_EQ(l, m.getLoc());
	EXPECT_EQ((Directions)3, m.getCaps());
}

