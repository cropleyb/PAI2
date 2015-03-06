#include "gmock/gmock.h"

#define private public

#include "bdebug.h"
#include "loc.h"
#include "line_lookup_table.h"

#include <string>
using std::string;
using testing::ElementsAre;

/*
 * These tests are for the LineLookupTable, which maps lengths of 5 to
 * significant pattern types.
 */

Mask maskStringToBs(const string &occStr)
{
    Mask ret = 0;
	int i = occStr.length() - 1;
	while (i >= 0) {
        ret *= 4;
		if (occStr[i] == 'B') ret += 1;
		else if (occStr[i] == 'W') ret += 2;
		else if (occStr[i] == '|') ret += 3;
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
	EXPECT_EQ(NoMatch, item->_matchType);
}

/////////////////////////////////////////////////////
// Lines
/////////////////////////////////////////////////////

TEST_F(LineLookupTableFixture, CountSingleBlack) {
	LineTableItem *item = processMaskString("B    ");
	EXPECT_EQ(P1, item->_colour);
	EXPECT_EQ(Line1, item->_matchType);
	ASSERT_THAT(item->_candInds, ElementsAre(1, 2, 3, 4));
}

TEST_F(LineLookupTableFixture, CountOneBlackOtherEnd) {
	LineTableItem *item = processMaskString("    B");
	EXPECT_EQ(P1, item->_colour);
	EXPECT_EQ(Line1, item->_matchType);
	ASSERT_THAT(item->_candInds, ElementsAre(0, 1, 2, 3));
}

TEST_F(LineLookupTableFixture, CountOneWhiteOtherEnd) {
	LineTableItem *item = processMaskString("    W");
	EXPECT_EQ(P2, item->_colour);
	ASSERT_THAT(item->_candInds, ElementsAre(0, 1, 2, 3));
}

TEST_F(LineLookupTableFixture, TwoBlacksAtStart) {
	LineTableItem *item = processMaskString("BB   ");
	EXPECT_EQ(P1, item->_colour);
	EXPECT_EQ(Line2, item->_matchType);
	ASSERT_THAT(item->_candInds, ElementsAre(2, 3, 4));
}

TEST_F(LineLookupTableFixture, MixedNoMatch) {
	LineTableItem *item = processMaskString("B   W");
	EXPECT_EQ(EMPTY, item->_colour);
	EXPECT_EQ(NoMatch, item->_matchType);
}

/////////////////////////////////////////////////////
// Takes
/////////////////////////////////////////////////////

TEST_F(LineLookupTableFixture, BlackTakeLeftEmpty) {
	LineTableItem *item = processMaskString("BWW  ");
	EXPECT_EQ(P1, item->_colour);
	EXPECT_EQ(Take, item->_matchType);
	ASSERT_THAT(item->_candInds, ElementsAre(3));
}

TEST_F(LineLookupTableFixture, WhiteTakeRightWhite) {
	LineTableItem *item = processMaskString(" BBWW");
	EXPECT_EQ(P2, item->_colour);
	EXPECT_EQ(Take, item->_matchType);
	ASSERT_THAT(item->_candInds, ElementsAre(0));
}

TEST_F(LineLookupTableFixture, WhiteTakeRightBlack) {
	LineTableItem *item = processMaskString(" BBWB");
	EXPECT_EQ(P2, item->_colour);
	EXPECT_EQ(Take, item->_matchType);
	ASSERT_THAT(item->_candInds, ElementsAre(0));
}

TEST_F(LineLookupTableFixture, WhiteTakeRightEdge) {
	LineTableItem *item = processMaskString(" BBW|");
	EXPECT_EQ(P2, item->_colour);
	EXPECT_EQ(Take, item->_matchType);
	ASSERT_THAT(item->_candInds, ElementsAre(0));
}

TEST_F(LineLookupTableFixture, WhiteTakeLeftEdge) {
	LineTableItem *item = processMaskString("WBB |");
	EXPECT_EQ(P2, item->_colour);
	EXPECT_EQ(Take, item->_matchType);
	ASSERT_THAT(item->_candInds, ElementsAre(3));
}

/////////////////////////////////////////////////////
// Threats
/////////////////////////////////////////////////////

TEST_F(LineLookupTableFixture, BlackThreatEmpty) {
	LineTableItem *item = processMaskString(" WW  ");
	EXPECT_EQ(P1, item->_colour);
	EXPECT_EQ(Threat, item->_matchType);
	ASSERT_THAT(item->_candInds, ElementsAre(0, 3));
}

TEST_F(LineLookupTableFixture, WhiteThreatWhite) {
	LineTableItem *item = processMaskString(" BB W");
	EXPECT_EQ(P2, item->_colour);
	EXPECT_EQ(Threat, item->_matchType);
	ASSERT_THAT(item->_candInds, ElementsAre(0, 3));
}

TEST_F(LineLookupTableFixture, WhiteThreatBlack) {
	LineTableItem *item = processMaskString(" BB B");
	EXPECT_EQ(P2, item->_colour);
	EXPECT_EQ(Threat, item->_matchType);
	ASSERT_THAT(item->_candInds, ElementsAre(0, 3));
}

TEST_F(LineLookupTableFixture, WhiteThreatEdge) {
	LineTableItem *item = processMaskString(" BB |");
	EXPECT_EQ(P2, item->_colour);
	EXPECT_EQ(Threat, item->_matchType);
	ASSERT_THAT(item->_candInds, ElementsAre(0, 3));
}

/////////////////////////////////////////////////////
// Blocked
/////////////////////////////////////////////////////

TEST_F(LineLookupTableFixture, BlackBlockedRight) {
	LineTableItem *item = processMaskString("BBBBW");
	EXPECT_EQ(P1, item->_colour);
	EXPECT_EQ(Blocked, item->_matchType);
	ASSERT_THAT(item->_candInds, ElementsAre());
}

TEST_F(LineLookupTableFixture, BlackBlockedLeft) {
	LineTableItem *item = processMaskString("WBBBB");
	EXPECT_EQ(P1, item->_colour);
	EXPECT_EQ(Blocked, item->_matchType);
	ASSERT_THAT(item->_candInds, ElementsAre());
}

TEST_F(LineLookupTableFixture, WhiteBlockedRight) {
	LineTableItem *item = processMaskString("WWWWB");
	EXPECT_EQ(P2, item->_colour);
	EXPECT_EQ(Blocked, item->_matchType);
	ASSERT_THAT(item->_candInds, ElementsAre());
}

TEST_F(LineLookupTableFixture, WhiteBlockedLeft) {
	LineTableItem *item = processMaskString("BWWWW");
	EXPECT_EQ(P2, item->_colour);
	EXPECT_EQ(Blocked, item->_matchType);
	ASSERT_THAT(item->_candInds, ElementsAre());
}

