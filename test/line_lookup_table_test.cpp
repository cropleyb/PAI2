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
	LinePattern *processMaskString(const string &occStr) 
	{
		buildLineLookupTable();
		Mask occs = maskStringToBs(occStr);
		return &lengthLookup[occs];
	}
	vector<Breadth> inds(LinePattern *lp)
	{
		vector<Breadth> res;
		for (int i=0; i<lp->_numCands; i++)
			res.push_back(lp->_inds[i]);
		return res;
	}
};


TEST_F(LineLookupTableFixture, NoLocsYet) {
	LinePattern *pattern = processMaskString("     ");
	EXPECT_EQ(EMPTY, pattern->_colour);
	EXPECT_EQ(NoPattern, pattern->_patternType);
}

/////////////////////////////////////////////////////
// Lines
/////////////////////////////////////////////////////

TEST_F(LineLookupTableFixture, CountSingleBlack) {
	LinePattern *pattern = processMaskString("B    ");
	EXPECT_EQ(P1, pattern->_colour);
	EXPECT_EQ(Line1, pattern->_patternType);
	ASSERT_THAT(inds(pattern), ElementsAre(1, 2, 3, 4));
}

TEST_F(LineLookupTableFixture, CountOneBlackOtherEnd) {
	LinePattern *pattern = processMaskString("    B");
	EXPECT_EQ(P1, pattern->_colour);
	EXPECT_EQ(Line1, pattern->_patternType);
	ASSERT_THAT(inds(pattern), ElementsAre(0, 1, 2, 3));
}

TEST_F(LineLookupTableFixture, CountOneWhiteOtherEnd) {
	LinePattern *pattern = processMaskString("    W");
	EXPECT_EQ(P2, pattern->_colour);
	ASSERT_THAT(inds(pattern), ElementsAre(0, 1, 2, 3));
}

TEST_F(LineLookupTableFixture, TwoBlacksAtStart) {
	LinePattern *pattern = processMaskString("BB   ");
	EXPECT_EQ(P1, pattern->_colour);
	EXPECT_EQ(Line2, pattern->_patternType);
	ASSERT_THAT(inds(pattern), ElementsAre(2, 3, 4));
}

TEST_F(LineLookupTableFixture, MixedNoMatch) {
	LinePattern *pattern = processMaskString("B   W");
	EXPECT_EQ(EMPTY, pattern->_colour);
	EXPECT_EQ(NoPattern, pattern->_patternType);
}

/////////////////////////////////////////////////////
// Takes
/////////////////////////////////////////////////////

TEST_F(LineLookupTableFixture, BlackTakeLeftEmpty) {
	LinePattern *pattern = processMaskString("BWW  ");
	EXPECT_EQ(P1, pattern->_colour);
	EXPECT_EQ(Take, pattern->_patternType);
	ASSERT_THAT(inds(pattern), ElementsAre(3));
}

TEST_F(LineLookupTableFixture, WhiteTakeRightWhite) {
	LinePattern *pattern = processMaskString(" BBWW");
	EXPECT_EQ(P2, pattern->_colour);
	EXPECT_EQ(Take, pattern->_patternType);
	ASSERT_THAT(inds(pattern), ElementsAre(0));
}

TEST_F(LineLookupTableFixture, WhiteTakeRightBlack) {
	LinePattern *pattern = processMaskString(" BBWB");
	EXPECT_EQ(P2, pattern->_colour);
	EXPECT_EQ(Take, pattern->_patternType);
	ASSERT_THAT(inds(pattern), ElementsAre(0));
}

TEST_F(LineLookupTableFixture, WhiteTakeRightEdge) {
	LinePattern *pattern = processMaskString(" BBW|");
	EXPECT_EQ(P2, pattern->_colour);
	EXPECT_EQ(Take, pattern->_patternType);
	ASSERT_THAT(inds(pattern), ElementsAre(0));
}

TEST_F(LineLookupTableFixture, WhiteTakeLeftEdge) {
	LinePattern *pattern = processMaskString("WBB |");
	EXPECT_EQ(P2, pattern->_colour);
	EXPECT_EQ(Take, pattern->_patternType);
	ASSERT_THAT(inds(pattern), ElementsAre(3));
}

/////////////////////////////////////////////////////
// Threats
/////////////////////////////////////////////////////

TEST_F(LineLookupTableFixture, BlackThreatEmpty) {
	LinePattern *pattern = processMaskString(" WW  ");
	EXPECT_EQ(P1, pattern->_colour);
	EXPECT_EQ(Threat, pattern->_patternType);
	ASSERT_THAT(inds(pattern), ElementsAre(0, 3));
}

TEST_F(LineLookupTableFixture, WhiteThreatWhite) {
	LinePattern *pattern = processMaskString(" BB W");
	EXPECT_EQ(P2, pattern->_colour);
	EXPECT_EQ(Threat, pattern->_patternType);
	ASSERT_THAT(inds(pattern), ElementsAre(0, 3));
}

TEST_F(LineLookupTableFixture, WhiteThreatBlack) {
	LinePattern *pattern = processMaskString(" BB B");
	EXPECT_EQ(P2, pattern->_colour);
	EXPECT_EQ(Threat, pattern->_patternType);
	ASSERT_THAT(inds(pattern), ElementsAre(0, 3));
}

TEST_F(LineLookupTableFixture, WhiteThreatEdge) {
	LinePattern *pattern = processMaskString(" BB |");
	EXPECT_EQ(P2, pattern->_colour);
	EXPECT_EQ(Threat, pattern->_patternType);
	ASSERT_THAT(inds(pattern), ElementsAre(0, 3));
}

/////////////////////////////////////////////////////
// Blocked
/////////////////////////////////////////////////////

TEST_F(LineLookupTableFixture, BlackBlockedRight) {
	LinePattern *pattern = processMaskString("BBBBW");
	EXPECT_EQ(P1, pattern->_colour);
	EXPECT_EQ(Blocked, pattern->_patternType);
	ASSERT_THAT(inds(pattern), ElementsAre());
}

TEST_F(LineLookupTableFixture, BlackBlockedLeft) {
	LinePattern *pattern = processMaskString("WBBBB");
	EXPECT_EQ(P1, pattern->_colour);
	EXPECT_EQ(Blocked, pattern->_patternType);
	ASSERT_THAT(inds(pattern), ElementsAre());
}

TEST_F(LineLookupTableFixture, WhiteBlockedRight) {
	LinePattern *pattern = processMaskString("WWWWB");
	EXPECT_EQ(P2, pattern->_colour);
	EXPECT_EQ(Blocked, pattern->_patternType);
	ASSERT_THAT(inds(pattern), ElementsAre());
}

TEST_F(LineLookupTableFixture, WhiteBlockedLeft) {
	LinePattern *pattern = processMaskString("BWWWW");
	EXPECT_EQ(P2, pattern->_colour);
	EXPECT_EQ(Blocked, pattern->_patternType);
	ASSERT_THAT(inds(pattern), ElementsAre());
}

