#define private public

#include "priority_level.h"
#include "gmock/gmock.h"
#include "bdebug.h"
#include "defines.h"

class PriorityLevelFixture : public testing::Test {
public:
	void arc(Loc l, int inc=1) { pl.addOrRemoveCandidate(l, inc); }

	PriorityLevel pl;
	Loc locBuffer[MAX_CANDS];
};

TEST_F(PriorityLevelFixture, testEmpty)
{
	Ind candCount = pl.getCands(locBuffer, 5);

	EXPECT_EQ(0, candCount);
}

TEST_F(PriorityLevelFixture, testAddOne)
{
	Loc l(5,3);
	arc(l);
	
	int candCount = pl.getCands(locBuffer, 5);

	EXPECT_EQ(1, candCount);
	EXPECT_EQ(l, locBuffer[0]);
}

// b priority_level_test.cpp:40
TEST_F(PriorityLevelFixture, testAddTwo)
{
	Loc l1(4,5);
	Loc l2(6,7);
	arc(l1);
	arc(l2);
	
	int candCount = pl.getCands(locBuffer, 5);

	EXPECT_EQ(2, candCount);
	EXPECT_EQ(l2, locBuffer[0]);
	EXPECT_EQ(l1, locBuffer[1]);
}

TEST_F(PriorityLevelFixture, testOneTwice)
{
	Loc l(4,5);
	arc(l);
	arc(l);
	
	int candCount = pl.getCands(locBuffer, 5);

	EXPECT_EQ(1, candCount);
	EXPECT_EQ(l, locBuffer[0]);
}

TEST_F(PriorityLevelFixture, testAddOneThenRemoveIt)
{
	Loc l(4,5);
	arc(l);
	arc(l, -1);
	
	int candCount = pl.getCands(locBuffer, 5);

	EXPECT_EQ(0, candCount);
}

TEST_F(PriorityLevelFixture, testAddTwoRemoveOne)
{
	Loc l1(4,5);
	Loc l2(6,7);
	arc(l1);
	arc(l2);
	arc(l1, -1);
	
	int candCount = pl.getCands(locBuffer, 5);

	EXPECT_EQ(1, candCount);
	EXPECT_EQ(l2, locBuffer[0]);
}

TEST_F(PriorityLevelFixture, testAddTooManyToIterateOver)
{
	Loc l1(1,1);
	Loc l2(2,2);
	Loc l3(3,3);
	arc(l1);
	arc(l2);
	arc(l3);
	
	int candCount = pl.getCands(locBuffer, 2);

	EXPECT_EQ(2, candCount);
	EXPECT_EQ(l3, locBuffer[0]);
	EXPECT_EQ(l2, locBuffer[1]);
}

TEST_F(PriorityLevelFixture, testDoubleComesFirstA)
{
	Loc l1(1,1);
	Loc l2(2,2);
	arc(l1);
	arc(l2);
	arc(l2);
	
	int candCount = pl.getCands(locBuffer, 2);

	EXPECT_EQ(2, candCount);
	EXPECT_EQ(l2, locBuffer[0]);
	EXPECT_EQ(l1, locBuffer[1]);
}

#if 0
// TODO keep them sorted by count?
TEST_F(PriorityLevelFixture, testDoubleComesFirstB)
{
	Loc l1(1,1);
	Loc l2(2,2);
	arc(l1);
	arc(l1);
	arc(l2);
	
	int candCount = pl.getCands(locBuffer, 2);

	EXPECT_EQ(2, candCount);
	EXPECT_EQ(l1, locBuffer[0]);
	EXPECT_EQ(l2, locBuffer[1]);
}
#endif
