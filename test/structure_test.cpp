#define private public

#include "structure.h"
#include "gmock/gmock.h"
#include "bdebug.h"
#include "defines.h"

class StructureFixture : public testing::Test {
public:
};

TEST_F(StructureFixture, testStructureAttributes)
{
	Structure s;
	EXPECT_EQ(NoPattern, s._pattern);

	s._pattern = Line4;
	s.addRelevant(Loc(0,0));
	EXPECT_EQ(1, s._numRelevant);
	EXPECT_EQ(Loc(0,0), s._relevant[0]);
}

TEST_F(StructureFixture, testAddOneToStructureMap)
{
	StructureMap sm;

	Structure &str = sm.add(Line4);
	sm.addRelevant(str, Loc(0,0));
	sm.addRelevant(str, Loc(0,1));
	sm.addRelevant(str, Loc(0,2));
	sm.addRelevant(str, Loc(0,3));

	EXPECT_EQ(4, str._numRelevant);
	EXPECT_EQ(Loc(0,0), str._relevant[0]);
	EXPECT_EQ(Loc(0,1), str._relevant[1]);
	EXPECT_EQ(Loc(0,2), str._relevant[2]);
	EXPECT_EQ(Loc(0,3), str._relevant[3]);
}

TEST_F(StructureFixture, testAddTwoToStructureMap)
{
	StructureMap sm;

	Structure &str1 = sm.add(Blocked);
	sm.addRelevant(str1, Loc(0,3));

	Structure &str2 = sm.add(Line4);
	sm.addRelevant(str2, Loc(1,3));

	EXPECT_EQ(Blocked, str1._pattern);
	// EXPECT_EQ(Loc(0,3), str._relevant[0]);
}

TEST_F(StructureFixture, testAddRemoveAdd)
{
	StructureMap sm;

	Structure &str1 = sm.add(Blocked);
    sm.remove(str1._id);
	EXPECT_EQ(NoPattern, str1._pattern);

	Structure &str2 = sm.add(Line4);
	sm.addRelevant(str2, Loc(1,3));
	EXPECT_EQ(str1._id, str2._id);
}
