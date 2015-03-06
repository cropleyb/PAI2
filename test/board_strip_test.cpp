
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

	void expectCandInd(LineTableItem &)
	{
	}

	template<typename ... Types>
	void expectCandInd(LineTableItem &lti, BoardWidth cand, Types ... rest)
	{
		lti._candInds.push_back(cand);
		expectCandInd(lti, rest...);
	}

	template<typename ... Types>
	void expectCandInds(LineTableItem &lti, Types ... rest)
	{
		lti._candInds.clear();
		expectCandInd(lti, rest...);
	}

	MockReporter mr;
	//LineTableItem lti;
};


TEST_F(BoardStripFixture, NoLocsYet) {
	processOccString("         |", 0, 9);
}

TEST_F(BoardStripFixture, OneMatch) {
	LineTableItem lti;
	lti._colour = P1;
	lti._matchType = Line1;
	expectCandInds(lti, 1,2,3,4);
	EXPECT_CALL(mr, report(lti));
	processOccString("B       |", 0, 8);
}

