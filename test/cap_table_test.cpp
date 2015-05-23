#include "gmock/gmock.h"

#define private public

#include "cap_table.h"

class CapTableFixture : public testing::Test {
public:
	CapTableFixture() {
	}

	CapTable ct;
};

TEST_F(CapTableFixture, addCapRecordsCapDirAndDist) {
   	// Trigger, direction from caps to trigger, offset, dist, add/sub
	ct.addOneCap(Loc(3,0), E_DIR, 1, 1, 1);
	CompressedLoc target1 = Loc(2,0)._value;
	EXPECT_EQ(1, ct._takes[target1]);
	EXPECT_EQ(true, ct.hasTakeInDirection(target1, E_DIR));
	
	ct.addOneCap(Loc(3,0), E_DIR, 1, 2, 1);
	CompressedLoc target2 = Loc(1,0)._value;
	EXPECT_EQ(2, ct._takes[target2]);
	EXPECT_EQ(true, ct.hasTakeInDirection(target2, E_DIR));
	
	ct.addOneCap(Loc(3,0), E_DIR, 1, 1, -1);
	EXPECT_EQ(0, ct._takes[target1]);
	EXPECT_EQ(false, ct.hasTakeInDirection(target1, E_DIR));

	ct.addOneCap(Loc(3,0), E_DIR, 1, 2, -1);
	EXPECT_EQ(0, ct._takes[target2]);
	EXPECT_EQ(false, ct.hasTakeInDirection(target2, E_DIR));
}

TEST_F(CapTableFixture, addCapRecordsCapDirAndDistDiffDir) {
	// Trigger, direction from caps, offset, dist, add/sub
	ct.addOneCap(Loc(1,4), N_DIR, 32, 1, 1);
	CompressedLoc target1 = Loc(1,3)._value;
	EXPECT_EQ(16, ct._takes[target1]);
	EXPECT_EQ(true, ct.hasTakeInDirection(target1, N_DIR));

	ct.addOneCap(Loc(1,4), N_DIR, 32, 2, 1);
	CompressedLoc target2 = Loc(1,2)._value;
	EXPECT_EQ(32, ct._takes[target2]);
	EXPECT_EQ(true, ct.hasTakeInDirection(target2, N_DIR));

	ct.addOneCap(Loc(1,4), N_DIR, 32, 1, -1);
	EXPECT_EQ(0, ct._takes[target1]);
	EXPECT_EQ(false, ct.hasTakeInDirection(target1, N_DIR));

	ct.addOneCap(Loc(1,4), N_DIR, 32, 2, -1);
	EXPECT_EQ(0, ct._takes[target2]);
	EXPECT_EQ(false, ct.hasTakeInDirection(target2, N_DIR));
}
