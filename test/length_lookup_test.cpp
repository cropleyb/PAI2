#include "gmock/gmock.h"

#define private public

#include "bdebug.h"
#include "loc.h"
#include "length_lookup.h"

#include <string>
using std::string;
using testing::ElementsAre;

Mask patternStringToBs(const string &occStr)
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

class MockStats
{
public:
    // MOCK_METHOD5(reportLengthCandidate, void(Colour colour, int length, int numLocs, Loc *locs, int inc));
	void reportLengthCandidate(Colour c, int length, int numLocs, Loc *locs, int inc)
	{
		if (c == P1) {
			blackCounter[length] += (numLocs * inc);
		} else {
			whiteCounter[length] += (numLocs * inc);
		}
		// TODO: set of locs.
	}
#if 0
    MOCK_CONST_METHOD0(isOneMove, bool());
	MOCK_METHOD0(getUtilityAndMove, std::pair<UtilityValue, Loc>());
    MOCK_METHOD0(makeNextMove, Loc());
    MOCK_METHOD0(undoLastMove, void());
    MOCK_CONST_METHOD1(isCutoff, bool(unsigned char depth));
#endif
	int blackCounter[5] = {0, 0, 0, 0, 0};
	int whiteCounter[5] = {0, 0, 0, 0, 0};
};

#if 0
template<MockStats>
void processSubstrips(Mask pattern, int firstInd, int lastInd, T *stats, Colour colour);
#endif

class LengthLookupFixture : public testing::Test {
public:
	MockStats ms;
	void processSubstripsString(const string &occStr, Colour colour) 
	{
		Mask occs = patternStringToBs(occStr);
		// processSubstrips(occs, 0, occStr.length()-1, &ms, colour);
	}
};

using ::testing::AtLeast;
using ::testing::Return;
using ::testing::InSequence;

TEST_F(LengthLookupFixture, NoLocsYet) {
	processSubstripsString("         ", P1);
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


#if 0
#!/usr/bin/env python

import unittest

from pentai.base import mock
from pentai.base import board_strip

from pentai.ai.length_lookup_table import *
from pentai.ai.utility_stats import UtilityStats

class StripCountingTest(unittest.TestCase):
    def setUp(self):
        self.util_stats = UtilityStats(search_filter=mock.Mock())
        self.black_counter = self.util_stats.lines[P1]
        self.white_counter = self.util_stats.lines[P2]

    # Helper
    def process_substrips_for_str(self, ss_str):
        pattern = pattern_string_to_bs(ss_str)
        us = self.util_stats
        bs_len = len(ss_str)
        process_substrips(pattern, 0, bs_len-1, us, 1)

    # Tests
    def test_count_empty(self):
        self.process_substrips_for_str("         ")
        self.assertEquals(self.black_counter, [0,0,0,0,0])
        self.assertEquals(self.white_counter, [0,0,0,0,0])

    def test_count_single_black(self):
        self.process_substrips_for_str("    B    ")
        self.assertEquals(self.black_counter, [5,0,0,0,0])
        self.assertEquals(self.white_counter, [0,0,0,0,0])

    def test_count_single_white(self):
        self.process_substrips_for_str("    W    ")
        self.assertEquals(self.black_counter, [0,0,0,0,0])
        self.assertEquals(self.white_counter, [5,0,0,0,0])

    def test_count_single_black_at_end(self):
        self.process_substrips_for_str("B        ")
        self.assertEquals(self.black_counter, [1,0,0,0,0])
        self.assertEquals(self.white_counter, [0,0,0,0,0])

    def test_count_open_three(self):
        self.process_substrips_for_str("   BBB   ")
        self.assertEquals(self.black_counter, [0,2,3,0,0])
        self.assertEquals(self.white_counter, [0,0,0,0,0])

    def test_count_open_four(self):
        self.process_substrips_for_str("  BBBB   ")
        self.assertEquals(self.black_counter, [0,1,2,2,0])
        self.assertEquals(self.white_counter, [0,0,0,0,0])

    def test_count_closed_four(self):
        self.process_substrips_for_str(" WBBBB   ")
        self.assertEquals(self.black_counter, [0,1,1,1,0])
        self.assertEquals(self.white_counter, [0,0,0,0,0])

    def test_count_open_three_with_space_and_single_white(self):
        self.process_substrips_for_str(" W BBB   ")
        self.assertEquals(self.black_counter, [0,1,2,0,0])
        self.assertEquals(self.white_counter, [0,0,0,0,0])

    def test_count_double_split_three(self):
        self.process_substrips_for_str("  B B B  ")
        self.assertEquals(self.black_counter, [0,4,1,0,0])
        self.assertEquals(self.white_counter, [0,0,0,0,0])

    def test_count_split_four(self):
        self.process_substrips_for_str("  BBB B  ")
        self.assertEquals(self.black_counter, [0,1,3,1,0])
        self.assertEquals(self.white_counter, [0,0,0,0,0])

    def test_count_middle_split_four(self):
        self.process_substrips_for_str("  BB BB  ")
        self.assertEquals(self.black_counter, [0,2,2,1,0])
        self.assertEquals(self.white_counter, [0,0,0,0,0])

    def test_count_five_mid(self):
        self.process_substrips_for_str("  BBBBB  ")
        self.assertEquals(self.black_counter, [0,0,2,2,1])
        self.assertEquals(self.white_counter, [0,0,0,0,0])

    def test_count_opponent_five(self):
        self.process_substrips_for_str("  WWWWW  ")
        self.assertEquals(self.black_counter, [0,0,0,0,0])
        self.assertEquals(self.white_counter, [0,0,2,2,1])

    def test_count_five_side(self):
        self.process_substrips_for_str("BBBBB    ")
        self.assertEquals(self.black_counter, [1,1,1,1,1])
        self.assertEquals(self.white_counter, [0,0,0,0,0])

    def test_count_five_side_with_stopper(self):
        self.process_substrips_for_str("BBBBBW   ")
        self.assertEquals(self.black_counter, [0,0,0,0,1])
        self.assertEquals(self.white_counter, [0,0,0,0,0])

    def test_count_three_side_with_stopper(self):
        self.process_substrips_for_str("BBB  W   ")
        self.assertEquals(self.black_counter, [0,0,1,0,0])
        self.assertEquals(self.white_counter, [2,0,0,0,0])

    def test_pre_threaten_pair(self):
        #st()
        self.process_substrips_for_str("     WW  ")
        self.assertEquals(self.black_counter, [0,0,0,0,0])
        self.assertEquals(self.white_counter, [1,3,0,0,0])

    def test_threaten_pair(self):
        self.process_substrips_for_str("    BWW  ")
        self.assertEquals(self.black_counter, [1,0,0,0,0])
        self.assertEquals(self.white_counter, [0,0,0,0,0])

    def test_threaten_two_pairs(self):
        self.process_substrips_for_str("  WWBWW  ")
        self.assertEquals(self.black_counter, [0,0,0,0,0])
        self.assertEquals(self.white_counter, [0,0,0,0,0])

    def test_block_four(self):
        self.process_substrips_for_str("    BWWWW")
        self.assertEquals(self.black_counter, [1,0,0,0,0])
        self.assertEquals(self.white_counter, [0,0,0,0,0])

    def test_too_short(self):
        self.process_substrips_for_str("W   ")
        self.assertEquals(self.black_counter, [0,0,0,0,0])
        self.assertEquals(self.white_counter, [0,0,0,0,0])

    def test_very_short(self):
        self.process_substrips_for_str("B")
        self.assertEquals(self.black_counter, [0,0,0,0,0])
        self.assertEquals(self.white_counter, [0,0,0,0,0])

    def test_diagonal_weirdness(self):
        self.process_substrips_for_str("  W  ")
        self.assertEquals(self.black_counter, [0,0,0,0,0])
        self.assertEquals(self.white_counter, [1,0,0,0,0])

'''
# These have been disabled due to cythonizing optimisation:
#   - making report_length_candidate into a cdef function

class CandidateReportingTest(unittest.TestCase):
    def setUp(self):
        self.black_counter = [0] * 5
        self.white_counter = [0] * 5
        self.util_stats = mock.Mock()
        self.real_report_length_candidate = \
                pentai.ai.utility_stats.report_length_candidate
        pentai.ai.utility_stats.report_length_candidate = mock.Mock

    def tearDown(self):
        pentai.ai.utility_stats.report_length_candidate = \
                self.real_report_length_candidate

    def process_substrips_for_str(self, ss_str):
        pattern = pattern_string_to_bs(ss_str)
        us = self.util_stats
        bs_len = len(ss_str)
        process_substrips(pattern, 0, bs_len-1, us, 1)

    def test_report_nothing(self):
        self.process_substrips_for_str("     ")
        calls = self.util_stats.mockGetAllCalls()
        self.assertEquals(len(calls),0)

    def test_report_a_four(self):
        self.process_substrips_for_str("BB BB")
        us = self.util_stats
        calls = us.mockGetAllCalls()
        self.assertEquals(len(calls),1)
        us.mockCheckCall(0, 'report_length_candidate', P1, 4, 0, [(2,0)], 1)

    def test_report_a_different_four(self):
        self.process_substrips_for_str("B BBB")
        us = self.util_stats
        calls = us.mockGetAllCalls()
        self.assertEquals(len(calls),1)
        us.mockCheckCall(0, 'report_length_candidate', P1, 4, [1], 1)

    def test_report_a_three(self):
        self.process_substrips_for_str("B B B")
        us = self.util_stats
        calls = us.mockGetAllCalls()
        self.assertEquals(len(calls),1)
        us.mockCheckCall(0, 'report_length_candidate', P1, 3, [1,3], 1)

    def test_report_a_three_and_a_two(self):
        self.process_substrips_for_str("B B B ")
        us = self.util_stats
        calls = us.mockGetAllCalls()
        self.assertEquals(len(calls),2)
        us.mockCheckCall(0, 'report_length_candidate', P1, 3, [1,3], 1)
        us.mockCheckCall(1, 'report_length_candidate', P1, 2, [3,1,5], 1)

    def test_report_a_white_four(self):
        self.process_substrips_for_str("WW WW")
        us = self.util_stats
        calls = us.mockGetAllCalls()
        self.assertEquals(len(calls),1)
        us.mockCheckCall(0, 'report_length_candidate', P2, 4, [2], 1)

    def test_report_a_four(self):
        self.process_substrips_for_str(" WB BBB")
        us = self.util_stats
        calls = us.mockGetAllCalls()
        self.assertEquals(len(calls),1)
        us.mockCheckCall(0, 'report_length_candidate', P1, 4, [3], 1)

    def test_report_a_five(self):
        self.process_substrips_for_str(" WBBBBB")
        us = self.util_stats
        calls = us.mockGetAllCalls()
        self.assertEquals(len(calls),1)
        us.mockCheckCall(0, 'report_length_candidate', P1, 5, [], 1)

    def atest_report_two_ones(self):
        # This is tricky because the empty list method of calculating subtypes
        # doesn't work for length 2? TODO
        self.process_substrips_for_str(" W    ")
        us = self.util_stats
        calls = us.mockGetAllCalls()
        self.assertEquals(len(calls),2)
        us.mockCheckCall(0, 'report_length_candidate', P2, 1, 0, [(2),(3),(0),(4)], 1)
        us.mockCheckCall(1, 'report_length_candidate', P2, 1, 0, [(3),(2),(4),(5)], 1)

    def atest_report_five_ones(self):
        self.process_substrips_for_str("    B    ")
        us = self.util_stats
        calls = us.mockGetAllCalls()
        self.assertEquals(len(calls),5)
        us.mockCheckCall(0, 'report_length_candidate', P1, 1, 0, [2,1,3,0], 1)
        us.mockCheckCall(1, 'report_length_candidate', P1, 1, 0, [3,2,1,5], 1)
        us.mockCheckCall(2, 'report_length_candidate', P1, 1, 0, [3,5,2,6], 1)
        us.mockCheckCall(3, 'report_length_candidate', P1, 1, 0, [5,6,3,7], 1)
        us.mockCheckCall(4, 'report_length_candidate', P1, 1, 0, [6,5,7,8], 1)
'''

if __name__ == "__main__":
    unittest.main()
#endif

#if 0
TEST_F(AlphaBetaFixture, FindFromOnlyOneOption) {
	// Do a search of a tree with a single possible move
	// - should find that move, and return it.
	// We shouldn't need to evaluate the utility function for that child
	// node, since there is only one choice.
	MockBridge mb;
	
    Loc locFromBridge(2,3);
	EXPECT_CALL(mb, isOneMove())
      .WillOnce(Return(true))
      ;
	EXPECT_CALL(mb, getOnlyMove())
      .WillOnce(Return(locFromBridge))
      ;

    AlphaBeta ab(mb);

	Loc move = ab.getBestMove();
	EXPECT_TRUE(move.isValid());
	EXPECT_EQ(move, locFromBridge);
}

TEST_F(AlphaBetaFixture, FindFromTwoOptions) {
	// Do a search of a tree with two possible moves, both terminal positions
	// - should find that move, and return it.
	// We need to evaluate the utility function for each terminal child node
	MockBridge mb;
	
    Loc locFromBridge1(2,3);
    std::pair<UtilityValue, Loc> ulpair1(20.5, locFromBridge1);
    Loc locFromBridge2(4,5);
    std::pair<UtilityValue, Loc> ulpair2(48.2, locFromBridge2);

	InSequence dummy;

	EXPECT_CALL(mb, isOneMove())
      .WillOnce(Return(false))
      ;
	EXPECT_CALL(mb, isCutoff(0))
      .WillOnce(Return(false))
      ;
	EXPECT_CALL(mb, makeNextMove())
      .WillOnce(Return(locFromBridge1))
      ;
	EXPECT_CALL(mb, isCutoff(1))
      .WillOnce(Return(true))
      ;
	EXPECT_CALL(mb, getUtilityAndMove())
      .WillOnce(Return(ulpair1))
      ;
	EXPECT_CALL(mb, undoLastMove())
      ;
	EXPECT_CALL(mb, makeNextMove())
      .WillOnce(Return(locFromBridge2))
      ;
	EXPECT_CALL(mb, isCutoff(1))
      .WillOnce(Return(true))
      ;
	EXPECT_CALL(mb, getUtilityAndMove())
      .WillOnce(Return(ulpair2))
      ;
	EXPECT_CALL(mb, undoLastMove())
      ;
	EXPECT_CALL(mb, makeNextMove())
      .WillOnce(Return(Loc::INVALID))
      ;

    AlphaBeta ab(mb);

	BD(cout << "1 before getBestMove" << endl);
	Loc move = ab.getBestMove();
	BD(cout << "x after getBestMove, (" << (int)move[0] << ',' << (int)move[1] << ')' << endl);
	BD(cout << "expected: " << locFromBridge2._value << endl);
	EXPECT_TRUE(move.isValid());
	EXPECT_EQ(locFromBridge2, move);
}

TEST_F(AlphaBetaFixture, FindFromTwoOptionsReversed) {
	// Same as above, only searching the best move first
	// - should find the first move, and return it.
	// We need to evaluate the utility function for each terminal child node
	MockBridge mb;
	
    Loc locFromBridge1(2,3);
    std::pair<UtilityValue, Loc> ulpair1(20.5, locFromBridge1);
    Loc locFromBridge2(4,5);
    std::pair<UtilityValue, Loc> ulpair2(17.2, locFromBridge2);

	InSequence dummy;

	EXPECT_CALL(mb, isOneMove())
      .WillOnce(Return(false))
      ;
	EXPECT_CALL(mb, isCutoff(0))
      .WillOnce(Return(false))
      ;
	EXPECT_CALL(mb, makeNextMove())
      .WillOnce(Return(locFromBridge1))
      ;
	EXPECT_CALL(mb, isCutoff(1))
      .WillOnce(Return(true))
      ;
	EXPECT_CALL(mb, getUtilityAndMove())
      .WillOnce(Return(ulpair1))
      ;
	EXPECT_CALL(mb, undoLastMove())
      ;
	EXPECT_CALL(mb, makeNextMove())
      .WillOnce(Return(locFromBridge2))
      ;
	EXPECT_CALL(mb, isCutoff(1))
      .WillOnce(Return(true))
      ;
	EXPECT_CALL(mb, getUtilityAndMove())
      .WillOnce(Return(ulpair2))
      ;
	EXPECT_CALL(mb, undoLastMove())
      ;
	EXPECT_CALL(mb, makeNextMove())
      .WillOnce(Return(Loc::INVALID))
      ;

    AlphaBeta ab(mb);

	BD(cout << "1 before getBestMove" << endl);
	Loc move = ab.getBestMove();
	BD(cout << "x after getBestMove, (" << (int)move[0] << ',' << (int)move[1] << ')' << endl);
	BD(cout << "expected: " << locFromBridge1._value << endl);
	EXPECT_TRUE(move.isValid());
	EXPECT_EQ(locFromBridge1, move);
}

TEST_F(AlphaBetaFixture, OpponentChoosesBadMoveForUs) {
	// Same as above, only searching the best move first
	// - should find the first move, and return it.
	// We need to evaluate the utility function for each terminal child node
	MockBridge mb;

    Loc loc1(1,1);
    std::pair<UtilityValue, Loc> ulpair1(1.0, loc1);
    Loc loc2(2,2);
    std::pair<UtilityValue, Loc> ulpair2(2.0, loc2);
    Loc loc3(3,3);
    std::pair<UtilityValue, Loc> ulpair3(3.0, loc3);
    Loc loc4(4,4);
    std::pair<UtilityValue, Loc> ulpair4(4.0, loc4);

	InSequence dummy;

	// This code is indented with the depth of the search.
	EXPECT_CALL(mb, isOneMove())
      .WillOnce(Return(false))
      ;
	EXPECT_CALL(mb, isCutoff(0))
      .WillOnce(Return(false))
      ;
	EXPECT_CALL(mb, makeNextMove())
      .WillOnce(Return(loc1))
      ;
		EXPECT_CALL(mb, isCutoff(1))
		  .WillOnce(Return(false))
		  ;
		EXPECT_CALL(mb, makeNextMove())
		  .WillOnce(Return(loc3))
		  ;
			EXPECT_CALL(mb, isCutoff(2))
			  .WillOnce(Return(true))
			  ;
			EXPECT_CALL(mb, getUtilityAndMove())
			  .WillOnce(Return(ulpair3))
			  ;
		EXPECT_CALL(mb, undoLastMove())
		  ;
		EXPECT_CALL(mb, makeNextMove())
		  .WillOnce(Return(loc4))
		  ;
			EXPECT_CALL(mb, isCutoff(2))
			  .WillOnce(Return(true))
			  ;
			EXPECT_CALL(mb, getUtilityAndMove())
			  .WillOnce(Return(ulpair4))
			  ;
		EXPECT_CALL(mb, undoLastMove())
		  ;
		EXPECT_CALL(mb, makeNextMove())
		  .WillOnce(Return(Loc::INVALID))
		  ;
	EXPECT_CALL(mb, undoLastMove())
      ;
	EXPECT_CALL(mb, makeNextMove())
      .WillOnce(Return(loc2))
      ;
		EXPECT_CALL(mb, isCutoff(1))
		  .WillOnce(Return(true))
		  ;
		EXPECT_CALL(mb, getUtilityAndMove())
		  .WillOnce(Return(ulpair2))
		  ;
	EXPECT_CALL(mb, undoLastMove())
      ;
	EXPECT_CALL(mb, makeNextMove())
	  .WillOnce(Return(Loc::INVALID))
	  ;

    AlphaBeta ab(mb);

	BD(cout << "1 before getBestMove" << endl);
	Loc move = ab.getBestMove();
	BD(cout << "x after getBestMove, (" << (int)move[0] << ',' << (int)move[1] << ')' << endl);
	EXPECT_TRUE(move.isValid());
	EXPECT_EQ(loc3, move);
}
#endif
