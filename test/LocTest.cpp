#include "gmock/gmock.h"

#define private public

#include "Loc.h"

class LocFixture : public testing::Test {
public:

};

TEST_F(LocFixture, CreateZeroLocInstance) {
    Loc loc(0, 0);
    EXPECT_EQ(loc._value, 0);
}

TEST_F(LocFixture, CreateNonZeroLocInstance) {
    Loc loc(2, 1);
    EXPECT_EQ(loc._value, 2 + 32);
}

TEST_F(LocFixture, CreateLargestLocInstance) {
    Loc loc(18, 18);
    EXPECT_EQ(loc._value, 18 + (32 * 18));
}
