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
// Fours
/////////////////////////////////////////////////////

TEST_F(LineLookupTableFixture, FourBlackLeft) {
	LinePattern *pattern = processMaskString("BBBB ");
	EXPECT_EQ(P1, pattern->_colour);
	EXPECT_EQ(Line4, pattern->_patternType);
	EXPECT_EQ(4, pattern->_inds[0]);
	ASSERT_THAT(0, pattern->occupied(0));
	ASSERT_THAT(1, pattern->occupied(1));
	ASSERT_THAT(2, pattern->occupied(2));
	ASSERT_THAT(3, pattern->occupied(3));
}

TEST_F(LineLookupTableFixture, FourWhiteRight) {
	LinePattern *pattern = processMaskString(" WWWW");
	EXPECT_EQ(P2, pattern->_colour);
	EXPECT_EQ(Line4, pattern->_patternType);
	EXPECT_EQ(0, pattern->_inds[0]);
	ASSERT_THAT(1, pattern->occupied(0));
	ASSERT_THAT(2, pattern->occupied(1));
	ASSERT_THAT(3, pattern->occupied(2));
	ASSERT_THAT(4, pattern->occupied(3));
}

TEST_F(LineLookupTableFixture, FourWhiteMiddle) {
	LinePattern *pattern = processMaskString("WW WW");
	EXPECT_EQ(P2, pattern->_colour);
	EXPECT_EQ(Line4, pattern->_patternType);
	EXPECT_EQ(2, pattern->_inds[0]);
	ASSERT_THAT(0, pattern->occupied(0));
	ASSERT_THAT(1, pattern->occupied(1));
	ASSERT_THAT(3, pattern->occupied(2));
	ASSERT_THAT(4, pattern->occupied(3));
}

TEST_F(LineLookupTableFixture, FourBlackMidLeft) {
	LinePattern *pattern = processMaskString("B BBB");
	EXPECT_EQ(P1, pattern->_colour);
	EXPECT_EQ(Line4, pattern->_patternType);
	EXPECT_EQ(1, pattern->_inds[0]);
	ASSERT_THAT(0, pattern->occupied(0));
	ASSERT_THAT(2, pattern->occupied(1));
	ASSERT_THAT(3, pattern->occupied(2));
	ASSERT_THAT(4, pattern->occupied(3));
}

TEST_F(LineLookupTableFixture, FourBlackMidRight) {
	LinePattern *pattern = processMaskString("BBB B");
	EXPECT_EQ(P1, pattern->_colour);
	EXPECT_EQ(Line4, pattern->_patternType);
	EXPECT_EQ(3, pattern->_inds[0]);
	ASSERT_THAT(0, pattern->occupied(0));
	ASSERT_THAT(1, pattern->occupied(1));
	ASSERT_THAT(2, pattern->occupied(2));
	ASSERT_THAT(4, pattern->occupied(3));
}

/////////////////////////////////////////////////////
// Takes
/////////////////////////////////////////////////////

TEST_F(LineLookupTableFixture, BlackTakeLeftEmpty) {
	LinePattern *pattern = processMaskString("BWW  ");
	EXPECT_EQ(P1, pattern->_colour);
	EXPECT_EQ(Take, pattern->_patternType);
	ASSERT_THAT(inds(pattern), ElementsAre(3));
	ASSERT_THAT(0, pattern->taker());
	ASSERT_THAT(1, pattern->victim(false));
	ASSERT_THAT(2, pattern->victim(true));
}

TEST_F(LineLookupTableFixture, WhiteTakeRightWhite) {
	LinePattern *pattern = processMaskString(" BBWW");
	EXPECT_EQ(P2, pattern->_colour);
	EXPECT_EQ(Take, pattern->_patternType);
	ASSERT_THAT(inds(pattern), ElementsAre(0));
	ASSERT_THAT(3, pattern->taker());
	ASSERT_THAT(1, pattern->victim(false));
	ASSERT_THAT(2, pattern->victim(true));
}

TEST_F(LineLookupTableFixture, WhiteTakeRightBlack) {
	LinePattern *pattern = processMaskString(" BBWB");
	EXPECT_EQ(P2, pattern->_colour);
	EXPECT_EQ(Take, pattern->_patternType);
	ASSERT_THAT(inds(pattern), ElementsAre(0));
	ASSERT_THAT(3, pattern->taker());
	ASSERT_THAT(1, pattern->victim(false));
	ASSERT_THAT(2, pattern->victim(true));
}

TEST_F(LineLookupTableFixture, WhiteTakeRightEdge) {
	LinePattern *pattern = processMaskString(" BBW|");
	EXPECT_EQ(P2, pattern->_colour);
	EXPECT_EQ(Take, pattern->_patternType);
	ASSERT_THAT(inds(pattern), ElementsAre(0));
	ASSERT_THAT(3, pattern->taker());
	ASSERT_THAT(1, pattern->victim(false));
	ASSERT_THAT(2, pattern->victim(true));
}

TEST_F(LineLookupTableFixture, WhiteTakeLeftEdge) {
	LinePattern *pattern = processMaskString("WBB |");
	EXPECT_EQ(P2, pattern->_colour);
	EXPECT_EQ(Take, pattern->_patternType);
	ASSERT_THAT(inds(pattern), ElementsAre(3));
	ASSERT_THAT(0, pattern->taker());
	ASSERT_THAT(1, pattern->victim(false));
	ASSERT_THAT(2, pattern->victim(true));
}

TEST_F(LineLookupTableFixture, CopyTakeInfo) {
	LinePattern *pattern = processMaskString("WBB |");
	LinePattern patt2(*pattern, 5);
	EXPECT_EQ(P2, patt2._colour);
	EXPECT_EQ(Take, patt2._patternType);
	ASSERT_THAT(inds(&patt2), ElementsAre(8));
	ASSERT_THAT(5, patt2.taker());
	ASSERT_THAT(6, patt2.victim(false));
	ASSERT_THAT(7, patt2.victim(true));
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

TEST_F(LineLookupTableFixture, BlackBlockedFarRight) {
	LinePattern *pattern = processMaskString("BBBBW");
	EXPECT_EQ(P1, pattern->_colour);
	EXPECT_EQ(Blocked, pattern->_patternType);
	ASSERT_THAT(inds(pattern), ElementsAre());
	ASSERT_THAT(4, pattern->blocker());
}

TEST_F(LineLookupTableFixture, BlackBlockedMidRight) {
	LinePattern *pattern = processMaskString("BBBWB");
	EXPECT_EQ(P1, pattern->_colour);
	EXPECT_EQ(Blocked, pattern->_patternType);
	ASSERT_THAT(inds(pattern), ElementsAre());
	ASSERT_THAT(3, pattern->blocker());
}

TEST_F(LineLookupTableFixture, BlackBlockedMiddle) {
	LinePattern *pattern = processMaskString("BBWBB");
	EXPECT_EQ(P1, pattern->_colour);
	EXPECT_EQ(Blocked, pattern->_patternType);
	ASSERT_THAT(inds(pattern), ElementsAre());
	ASSERT_THAT(2, pattern->blocker());
}

TEST_F(LineLookupTableFixture, BlackBlockedMidLeft) {
	LinePattern *pattern = processMaskString("BWBBB");
	EXPECT_EQ(P1, pattern->_colour);
	EXPECT_EQ(Blocked, pattern->_patternType);
	ASSERT_THAT(inds(pattern), ElementsAre());
	ASSERT_THAT(1, pattern->blocker());
}

TEST_F(LineLookupTableFixture, BlackBlockedFarLeft) {
	LinePattern *pattern = processMaskString("WBBBB");
	EXPECT_EQ(P1, pattern->_colour);
	EXPECT_EQ(Blocked, pattern->_patternType);
	ASSERT_THAT(inds(pattern), ElementsAre());
	ASSERT_THAT(0, pattern->blocker());
}

TEST_F(LineLookupTableFixture, WhiteBlockedFarRight) {
	LinePattern *pattern = processMaskString("WWWWB");
	EXPECT_EQ(P2, pattern->_colour);
	EXPECT_EQ(Blocked, pattern->_patternType);
	ASSERT_THAT(inds(pattern), ElementsAre());
	ASSERT_THAT(4, pattern->blocker());
}

TEST_F(LineLookupTableFixture, WhiteBlockedMidRight) {
	LinePattern *pattern = processMaskString("WWWBW");
	EXPECT_EQ(P2, pattern->_colour);
	EXPECT_EQ(Blocked, pattern->_patternType);
	ASSERT_THAT(inds(pattern), ElementsAre());
	ASSERT_THAT(3, pattern->blocker());
}

TEST_F(LineLookupTableFixture, WhiteBlockedMiddle) {
	LinePattern *pattern = processMaskString("WWBWW");
	EXPECT_EQ(P2, pattern->_colour);
	EXPECT_EQ(Blocked, pattern->_patternType);
	ASSERT_THAT(inds(pattern), ElementsAre());
	ASSERT_THAT(2, pattern->blocker());
}

TEST_F(LineLookupTableFixture, WhiteBlockedMidLeft) {
	LinePattern *pattern = processMaskString("WBWWW");
	EXPECT_EQ(P2, pattern->_colour);
	EXPECT_EQ(Blocked, pattern->_patternType);
	ASSERT_THAT(inds(pattern), ElementsAre());
	ASSERT_THAT(1, pattern->blocker());
}

TEST_F(LineLookupTableFixture, WhiteBlockedFarLeft) {
	LinePattern *pattern = processMaskString("BWWWW");
	EXPECT_EQ(P2, pattern->_colour);
	EXPECT_EQ(Blocked, pattern->_patternType);
	ASSERT_THAT(inds(pattern), ElementsAre());
	ASSERT_THAT(0, pattern->blocker());
}

