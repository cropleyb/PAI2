#include "gmock/gmock.h"

#define private public

#include "bdebug.h"
#include "loc.h"
#include "line_lookup_table.h"

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

class LineLookupTableFixture : public testing::Test {
public:
	LineTableItem *processMaskString(const string &occStr) 
	{
		buildAll();
		Mask occs = maskStringToBs(occStr);
		return &lengthLookup[occs];
	}
};


TEST_F(LineLookupTableFixture, NoLocsYet) {
	LineTableItem *item = processMaskString("     ");
	EXPECT_EQ(EMPTY, item->_colour);
	EXPECT_EQ(0, item->_length);
}

TEST_F(LineLookupTableFixture, CountSingleBlack) {
	LineTableItem *item = processMaskString("B    ");
	EXPECT_EQ(P1, item->_colour);
	EXPECT_EQ(1, item->_length);
	ASSERT_THAT(item->_candInds, ElementsAre(1, 2, 3, 4));
}

TEST_F(LineLookupTableFixture, CountOneBlackOtherEnd) {
	LineTableItem *item = processMaskString("    B");
	EXPECT_EQ(P1, item->_colour);
	EXPECT_EQ(1, item->_length);
	ASSERT_THAT(item->_candInds, ElementsAre(0, 1, 2, 3));
}

TEST_F(LineLookupTableFixture, CountOneWhiteOtherEnd) {
	LineTableItem *item = processMaskString("    W");
	EXPECT_EQ(P2, item->_colour);
	EXPECT_EQ(1, item->_length);
	ASSERT_THAT(item->_candInds, ElementsAre(0, 1, 2, 3));
}

TEST_F(LineLookupTableFixture, TwoBlacksAtStart) {
	LineTableItem *item = processMaskString("BB   ");
	EXPECT_EQ(P1, item->_colour);
	EXPECT_EQ(2, item->_length);
	ASSERT_THAT(item->_candInds, ElementsAre(2, 3, 4));
}

TEST_F(LineLookupTableFixture, MixedNoMatch) {
	LineTableItem *item = processMaskString("B   W");
	EXPECT_EQ(EMPTY, item->_colour);
	EXPECT_EQ(0, item->_length);
}
