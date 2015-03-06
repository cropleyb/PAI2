
#include "gmock/gmock.h"

#define private public

#include "bdebug.h"
#include "loc.h"
#include "line_lookup_table.h"
#include "board_strip.h"

#include <string>
using std::string;
using testing::ElementsAre;

/*
 * These tests are for matching the sections of a board strip line, around
 * a given index. Significant pattern types are reported with relevant indices.
 */

U64 occStringToBs(const string &occStr)
{
    U64 ret = 0;
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

class MockReporter
{
public:
	MockReporter() {}

    MOCK_METHOD1(report, void(const LineTableItem &));
};

using ::testing::AtLeast;
using ::testing::Return;
using ::testing::InSequence;

class BoardStripFixture : public testing::Test {
public:
	BoardStripFixture() {
		buildAll();
	}

	void processOccString(const string &occStr, BoardWidth leftInd, BoardWidth rightInd) 
	{
		U64 occs = occStringToBs(occStr);
		matchRange(occs, leftInd, rightInd, mr);
	}

	//void expectCandInds(BoardWidth 

	MockReporter mr;
	LineTableItem lti;
};


TEST_F(BoardStripFixture, NoLocsYet) {
	processOccString("         |", 0, 9);
}

TEST_F(BoardStripFixture, OneMatch) {
	lti._colour = P1;
	lti._matchType = Line1;
	lti._candInds.push_back(1);
	lti._candInds.push_back(2);
	lti._candInds.push_back(3);
	lti._candInds.push_back(4);
	EXPECT_CALL(mr, report(lti));
	processOccString("B       |", 0, 8);
}

#if 0
TEST_F(AlphaBetaFixture, NoOptions) {
	// Do a search of a tree with no possible moves
	// - should return an invalid Loc instance
	MockBridge mb;
	
	EXPECT_CALL(mb, isOneMove())
      .WillOnce(Return(true))
      ;
	EXPECT_CALL(mb, getOnlyMove())
      .WillOnce(Return(Loc::INVALID))
      ;

    AlphaBeta ab(mb);

	Loc move = ab.getBestMove();
	EXPECT_EQ(move.isValid(), false);
}

/////////////////////////////////////////////////////
// Lines
/////////////////////////////////////////////////////

TEST_F(BoardStripFixture, CountSingleBlack) {
	LineTableItem *item = processOccString("B    ");
	EXPECT_EQ(P1, item->_colour);
	EXPECT_EQ(Line1, item->_matchType);
	ASSERT_THAT(item->_candInds, ElementsAre(1, 2, 3, 4));
}

TEST_F(BoardStripFixture, CountOneBlackOtherEnd) {
	LineTableItem *item = processOccString("    B");
	EXPECT_EQ(P1, item->_colour);
	EXPECT_EQ(Line1, item->_matchType);
	ASSERT_THAT(item->_candInds, ElementsAre(0, 1, 2, 3));
}

TEST_F(BoardStripFixture, CountOneWhiteOtherEnd) {
	LineTableItem *item = processOccString("    W");
	EXPECT_EQ(P2, item->_colour);
	ASSERT_THAT(item->_candInds, ElementsAre(0, 1, 2, 3));
}

TEST_F(BoardStripFixture, TwoBlacksAtStart) {
	LineTableItem *item = processOccString("BB   ");
	EXPECT_EQ(P1, item->_colour);
	EXPECT_EQ(Line2, item->_matchType);
	ASSERT_THAT(item->_candInds, ElementsAre(2, 3, 4));
}

TEST_F(BoardStripFixture, MixedNoMatch) {
	LineTableItem *item = processOccString("B   W");
	EXPECT_EQ(EMPTY, item->_colour);
	EXPECT_EQ(NoMatch, item->_matchType);
}

/////////////////////////////////////////////////////
// Takes
/////////////////////////////////////////////////////

TEST_F(BoardStripFixture, BlackTakeLeftEmpty) {
	LineTableItem *item = processOccString("BWW  ");
	EXPECT_EQ(P1, item->_colour);
	EXPECT_EQ(Take, item->_matchType);
	ASSERT_THAT(item->_candInds, ElementsAre(3));
}

TEST_F(BoardStripFixture, WhiteTakeRightWhite) {
	LineTableItem *item = processOccString(" BBWW");
	EXPECT_EQ(P2, item->_colour);
	EXPECT_EQ(Take, item->_matchType);
	ASSERT_THAT(item->_candInds, ElementsAre(0));
}

TEST_F(BoardStripFixture, WhiteTakeRightBlack) {
	LineTableItem *item = processOccString(" BBWB");
	EXPECT_EQ(P2, item->_colour);
	EXPECT_EQ(Take, item->_matchType);
	ASSERT_THAT(item->_candInds, ElementsAre(0));
}

TEST_F(BoardStripFixture, WhiteTakeRightEdge) {
	LineTableItem *item = processOccString(" BBW|");
	EXPECT_EQ(P2, item->_colour);
	EXPECT_EQ(Take, item->_matchType);
	ASSERT_THAT(item->_candInds, ElementsAre(0));
}

TEST_F(BoardStripFixture, WhiteTakeLeftEdge) {
	LineTableItem *item = processOccString("WBB |");
	EXPECT_EQ(P2, item->_colour);
	EXPECT_EQ(Take, item->_matchType);
	ASSERT_THAT(item->_candInds, ElementsAre(3));
}

/////////////////////////////////////////////////////
// Threats
/////////////////////////////////////////////////////

TEST_F(BoardStripFixture, BlackThreatEmpty) {
	LineTableItem *item = processOccString(" WW  ");
	EXPECT_EQ(P1, item->_colour);
	EXPECT_EQ(Threat, item->_matchType);
	ASSERT_THAT(item->_candInds, ElementsAre(0, 3));
}

TEST_F(BoardStripFixture, WhiteThreatWhite) {
	LineTableItem *item = processOccString(" BB W");
	EXPECT_EQ(P2, item->_colour);
	EXPECT_EQ(Threat, item->_matchType);
	ASSERT_THAT(item->_candInds, ElementsAre(0, 3));
}

TEST_F(BoardStripFixture, WhiteThreatBlack) {
	LineTableItem *item = processOccString(" BB B");
	EXPECT_EQ(P2, item->_colour);
	EXPECT_EQ(Threat, item->_matchType);
	ASSERT_THAT(item->_candInds, ElementsAre(0, 3));
}

TEST_F(BoardStripFixture, WhiteThreatEdge) {
	LineTableItem *item = processOccString(" BB |");
	EXPECT_EQ(P2, item->_colour);
	EXPECT_EQ(Threat, item->_matchType);
	ASSERT_THAT(item->_candInds, ElementsAre(0, 3));
}

/////////////////////////////////////////////////////
// Blocked
/////////////////////////////////////////////////////

TEST_F(BoardStripFixture, BlackBlockedRight) {
	LineTableItem *item = processOccString("BBBBW");
	EXPECT_EQ(P1, item->_colour);
	EXPECT_EQ(Blocked, item->_matchType);
	ASSERT_THAT(item->_candInds, ElementsAre());
}

TEST_F(BoardStripFixture, BlackBlockedLeft) {
	LineTableItem *item = processOccString("WBBBB");
	EXPECT_EQ(P1, item->_colour);
	EXPECT_EQ(Blocked, item->_matchType);
	ASSERT_THAT(item->_candInds, ElementsAre());
}

TEST_F(BoardStripFixture, WhiteBlockedRight) {
	LineTableItem *item = processOccString("WWWWB");
	EXPECT_EQ(P2, item->_colour);
	EXPECT_EQ(Blocked, item->_matchType);
	ASSERT_THAT(item->_candInds, ElementsAre());
}

TEST_F(BoardStripFixture, WhiteBlockedLeft) {
	LineTableItem *item = processOccString("BWWWW");
	EXPECT_EQ(P2, item->_colour);
	EXPECT_EQ(Blocked, item->_matchType);
	ASSERT_THAT(item->_candInds, ElementsAre());
}
#endif
