#include "gmock/gmock.h"

#define private public

#include "bdebug.h"
#include "loc.h"
#include "length_lookup_table.h"

#include <string>
using std::string;
using testing::ElementsAre;

Mask maskStringToBs(const string &occStr)
{
    Mask ret = 0;
	int i = occStr.length() - 1;
	while (i >= 0) {
        ret *= 4;
		if (occStr[i] == 'B') ret += 1;
		else if (occStr[i] == 'W') ret += 2;
		i -= 1;
	}
    return ret;
}

class LengthLookupTableFixture : public testing::Test {
public:
	LengthTableItem *processMaskString(const string &occStr) 
	{
		buildAll();
		Mask occs = maskStringToBs(occStr);
		return &lengthLookup[occs];
	}
};


TEST_F(LengthLookupTableFixture, NoLocsYet) {
	LengthTableItem *item = processMaskString("     ");
	EXPECT_EQ(EMPTY, item->_colour);
	EXPECT_EQ(0, item->_length);
}

TEST_F(LengthLookupTableFixture, CountSingleBlack) {
	LengthTableItem *item = processMaskString("B    ");
	EXPECT_EQ(P1, item->_colour);
	EXPECT_EQ(1, item->_length);
	ASSERT_THAT(item->_empty, ElementsAre(false,true,true,true,true));
}

TEST_F(LengthLookupTableFixture, CountOneBlackOtherEnd) {
	LengthTableItem *item = processMaskString("    B");
	EXPECT_EQ(P1, item->_colour);
	EXPECT_EQ(1, item->_length);
	ASSERT_THAT(item->_empty, ElementsAre(true,true,true,true,false));
}

TEST_F(LengthLookupTableFixture, CountOneWhiteOtherEnd) {
	LengthTableItem *item = processMaskString("    W");
	EXPECT_EQ(P2, item->_colour);
	EXPECT_EQ(1, item->_length);
	ASSERT_THAT(item->_empty, ElementsAre(true,true,true,true,false));
}

TEST_F(LengthLookupTableFixture, TwoBlacksAtStart) {
	LengthTableItem *item = processMaskString("BB   ");
	EXPECT_EQ(P1, item->_colour);
	EXPECT_EQ(2, item->_length);
	ASSERT_THAT(item->_empty, ElementsAre(false,false,true,true,true));
}

TEST_F(LengthLookupTableFixture, MixedNoMatch) {
	LengthTableItem *item = processMaskString("B   W");
	EXPECT_EQ(EMPTY, item->_colour);
	EXPECT_EQ(0, item->_length);
}

