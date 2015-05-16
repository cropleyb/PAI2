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
	ct.addCap(Loc(0,0), E_DIR, 1, 1); // Trigger, direction from caps, offset, add/sub
	EXPECT_EQ(1, ct._takes[Loc(1,0)._value]);
	EXPECT_EQ(2, ct._takes[Loc(2,0)._value]);
	ct.addCap(Loc(0,0), E_DIR, 1, -1);
	EXPECT_EQ(0, ct._takes[Loc(1,0)._value]);
	EXPECT_EQ(0, ct._takes[Loc(2,0)._value]);
}

TEST_F(CapTableFixture, addCapRecordsCapDirAndDistDiffDir) {
	ct.addCap(Loc(0,0), N_DIR, 32, 1); // Trigger, direction from caps, offset, add/sub
	EXPECT_EQ(16, ct._takes[Loc(0,1)._value]);
	EXPECT_EQ(32, ct._takes[Loc(0,2)._value]);
	ct.addCap(Loc(0,0), N_DIR, 32, -1);
	EXPECT_EQ(0, ct._takes[Loc(0,1)._value]);
	EXPECT_EQ(0, ct._takes[Loc(0,2)._value]);
}
