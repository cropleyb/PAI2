#include "gmock/gmock.h"

#define private public

#include "Loc.h"

class LocFixture : public testing::Test {
public:

};

TEST_F(LocFixture, CreateZeroLocInstance) {
    Loc loc(0, 0);
    EXPECT_EQ(loc._value, 0);
    EXPECT_TRUE(loc.isValid());
}

TEST_F(LocFixture, CreateNonZeroLocInstance) {
    Loc loc(2, 1);
    EXPECT_EQ(loc._value, 2 + 32);
    EXPECT_TRUE(loc.isValid());
}

TEST_F(LocFixture, CreateLargestLocInstance) {
    Loc loc(18, 18);
    EXPECT_EQ(loc._value, 18 + (32 * 18));
    EXPECT_TRUE(loc.isValid());
}

TEST_F(LocFixture, DefaultLocIsInvalid) {
    Loc loc;
    EXPECT_FALSE(loc.isValid());
}

TEST_F(LocFixture, INVALIDIsInvalid) {
    EXPECT_FALSE(Loc::INVALID.isValid());
}

TEST_F(LocFixture, GetCoord1And2) {
    Loc loc(3, 5);
    int x = loc[0];
    int y = loc[1];
    EXPECT_EQ(3, x);
    EXPECT_EQ(5, y);
}

TEST_F(LocFixture, WhatsGoingOnWithNegValue) {
    Loc loc(4, 0);
    int x = loc[0];
    int y = loc[1];
    EXPECT_EQ(4, x);
    EXPECT_EQ(0, y);
    EXPECT_EQ(4, loc._value);
}
