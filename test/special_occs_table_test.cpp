#include "gmock/gmock.h"

#define private public

#include "special_occs_table.h"

class SpecialOccsTableFixture : public testing::Test {
public:
	SpecialOccsTableFixture() {
	}

	SpecialOccsTable sot;
};

TEST_F(SpecialOccsTableFixture, CountUnion) {
	SpecialOccCounts soc;
	soc.all = 0;

	soc.fours += 15;
	EXPECT_EQ(15, soc.fours);
	EXPECT_EQ(0, soc.blocked4s);
	EXPECT_EQ(0, soc.takes);
	EXPECT_LT(0, soc.all);
	soc.all = 0;
	EXPECT_EQ(0, soc.all);

	soc.blocked4s += 15;
	EXPECT_EQ(15, soc.blocked4s);
	EXPECT_EQ(0, soc.fours);
	EXPECT_EQ(0, soc.takes);
	EXPECT_LT(0, soc.all);
	soc.all = 0;
	EXPECT_EQ(0, soc.all);

	soc.takes += 7;
	EXPECT_EQ(7, soc.takes);
	EXPECT_EQ(0, soc.blocked4s);
	EXPECT_EQ(0, soc.fours);
	EXPECT_LT(0, soc.all);
	soc.all = 0;
	EXPECT_EQ(0, soc.all);
}

TEST_F(SpecialOccsTableFixture, setCounts) {
	EXPECT_EQ(0, sot._counts[0].all);
	EXPECT_EQ(0, sot._counts[MAX_LOCS-1].all);
}
