#include "gmock/gmock.h"

#define private public

#include "bdebug.h"
#include "defines.h"
#include "loc.h"
#include "position_stats.h"

// #include <string>
using std::string;
//using testing::ElementsAre;

class PositionStatsFixture : public testing::Test {
public:
	PositionStats ps;
#if 0
	MockStats ms;
	void processSubstripsString(const string &occStr, Colour colour) 
	{
		Mask occs = patternStringToBs(occStr);
		// processSubstrips(occs, 0, occStr.length()-1, &ms, colour);
	}
#endif
};

using ::testing::AtLeast;
using ::testing::Return;
using ::testing::InSequence;

using std::vector;
typedef vector<Loc> LocArr;

TEST_F(PositionStatsFixture, NoStatsYet) {
	LocArr ll;
	ll.push_back(Loc(5,2));
	ps.reportLengthCandidates(P1, 1, ll, 1);
	// Ind getCands(Loc *locBuffer, Ind *countBuffer, Ind max);
	// Ind getNumCands();
	EXPECT_EQ(1, ps.getLengthPriorityLevel(P1, 1).getNumCands());
	
	//ASSERT_THAT(ms.blackCounter, ElementsAre(5,0,0,0,0));
}

#if 0
// TODO...
TEST_F(LengthLookupFixture, CountSingleBlack) {
	// Do a search of a tree with no possible moves
	// - should return an invalid Loc instance
	// MockStats ms;
	
	// EXPECT_CALL(ms, reportLengthCandidate(P1, 1, 1, ms.whatever, 1));
	  // .WillOnce(Return(Loc::INVALID))
    //MOCK_METHOD5(reportLengthCandidate, void(Colour colour, int length, int numLocs, Loc *locs, int inc));
	// template<MockStats> 
	//processSubstrips(&ms); //int firstInd, int lastInd, T *stats, Colour colour)
	processSubstripsString("    B    ", P1);
	ASSERT_THAT(ms.blackCounter, ElementsAre(5,0,0,0,0));
	ASSERT_THAT(ms.whiteCounter, ElementsAre(0,0,0,0,0));
}
#endif
