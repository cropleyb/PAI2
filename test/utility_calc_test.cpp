#include "gmock/gmock.h"

#define private public

//#include "position_stats.h"
#include "utility_calc.h"
#include "line_pattern.h"
#include "defines.h"
#include "bdebug.h"

#define SMALL_INF 1e+15

class MockPositionStats
{
public:
    MockPositionStats()
	{
		for (int c=P1; c<=P2; c+=1)
		{
			for (int patt=0; patt<MAX_PATTERN_TYPE; patt+=1)
			{
				_patternCounts[c][patt] = 0;
			}
			_captured[c] = 0;
		}
	}

	// TODO: checkerboardStats
	
	CapCount _captured[3];
	PattCount _patternCounts[3][MAX_PATTERN_TYPE];
};

class UtilityCalcFixture : public testing::Test {
public:
	UtilityCalcFixture() : uc(ps)
	{
	}

	void setLineCounts(Colour p, int line1, int line2, int line3, int line4, int line5)
	{
		ps._patternCounts[p][Line1] = line1;
		ps._patternCounts[p][Line2] = line2;
		ps._patternCounts[p][Line3] = line3;
		ps._patternCounts[p][Line4] = line4;
		ps._patternCounts[p][Line5] = line5;
	}

    void setCaptured(CapCount p1caps, CapCount p2caps)
	{
		ps._captured[P1] = p1caps;
		ps._captured[P2] = p2caps;
	}

	void setTakes(PattCount t1, PattCount t2)
	{
		ps._patternCounts[P1][Take] = t1;
		ps._patternCounts[P2][Take] = t2;
	}

	void setThreats(PattCount t1, PattCount t2)
	{
		ps._patternCounts[P1][Threat] = t1;
		ps._patternCounts[P2][Threat] = t2;
	}

    MockPositionStats ps;
	UtilityCalc<MockPositionStats> uc;
};

////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////
TEST_F(UtilityCalcFixture, AOneIsBetterThanNothing) {
	setLineCounts(P1, 1,0,0,0,0);
	setLineCounts(P2, 0,0,0,0,0);
	UtilityValue u = uc.calcUtility(P1, P1, 1);
	EXPECT_LT(0, u);
}

TEST_F(UtilityCalcFixture, ATwoIsBetterThanNothing) {
	setLineCounts(P1, 0,1,0,0,0);
	setLineCounts(P2, 0,0,0,0,0);
	UtilityValue u = uc.calcUtility(P1, P1, 1);
	EXPECT_LT(0, u);
}

TEST_F(UtilityCalcFixture, AThreeIsBetterThanNothing) {
	setLineCounts(P1, 0,0,1,0,0);
	setLineCounts(P2, 0,0,0,0,0);
	UtilityValue u = uc.calcUtility(P1, P1, 1);
	EXPECT_LT(0, u);
}

TEST_F(UtilityCalcFixture, AFourIsBetterThanNothing) {
	setLineCounts(P1, 0,0,0,1,0);
	setLineCounts(P2, 0,0,0,0,0);
	UtilityValue u = uc.calcUtility(P1, P1, 1);
	EXPECT_LT(0, u);
}

TEST_F(UtilityCalcFixture, TheirOneIsWorseThanNothing) {
	setLineCounts(P1, 0,0,0,0,0);
	setLineCounts(P2, 1,0,0,0,0);
	UtilityValue u = uc.calcUtility(P1, P1, 1);
	EXPECT_GT(0, u);
}

TEST_F(UtilityCalcFixture, TheirTwoIsWorseThanNothing) {
	setLineCounts(P1, 0,0,0,0,0);
	setLineCounts(P2, 0,1,0,0,0);
	UtilityValue u = uc.calcUtility(P1, P1, 1);
	EXPECT_GT(0, u);
}

TEST_F(UtilityCalcFixture, TheirThreeIsWorseThanNothing) {
	setLineCounts(P1, 0,0,0,0,0);
	setLineCounts(P2, 0,0,1,0,0);
	UtilityValue u = uc.calcUtility(P1, P1, 1);
	EXPECT_GT(0, u);
}

TEST_F(UtilityCalcFixture, TheirFourIsWorseThanNothing) {
	setLineCounts(P1, 0,0,0,0,0);
	setLineCounts(P2, 0,0,0,1,0);
	UtilityValue u = uc.calcUtility(P1, P1, 1);
	EXPECT_GT(0, u);
}

TEST_F(UtilityCalcFixture, AFiveWins) {
	setLineCounts(P1, 0,0,0,0,1);
	setLineCounts(P2, 0,0,0,0,0);
	UtilityValue u = uc.calcUtility(P1, P1, 1);
	EXPECT_LT(SMALL_INF, u);
}

TEST_F(UtilityCalcFixture, TheirFiveLoses) {
	setLineCounts(P1, 0,0,0,0,0);
	setLineCounts(P2, 0,0,0,0,1);
	UtilityValue u = uc.calcUtility(P1, P1, 1);
	EXPECT_GT(-SMALL_INF, u);
}

TEST_F(UtilityCalcFixture, P1WinByCaptures) {
	setLineCounts(P1, 0,0,0,0,0);
	setLineCounts(P2, 0,0,0,0,0);
    setCaptured(10, 0);
	UtilityValue u = uc.calcUtility(P1, P1, 1);
	EXPECT_LT(SMALL_INF, u);
}

TEST_F(UtilityCalcFixture, testWonGameShortening) {
	setLineCounts(P1, 0,0,0,4,0);

	UtilityValue u1 = uc.calcUtility(P1, P1, 10);
	UtilityValue u2 = uc.calcUtility(P1, P1, 11);

	EXPECT_GT(u1, u2);
}

TEST_F(UtilityCalcFixture, testLostGameLengthening) {
	setLineCounts(P2, 0,0,0,4,0);

	UtilityValue u1 = uc.calcUtility(P2, P1, 10);
	UtilityValue u2 = uc.calcUtility(P2, P1, 11);

	EXPECT_GT(u2, u1);
}

TEST_F(UtilityCalcFixture, testP2WinByCaptures) {
	setCaptured(0, 10);
	UtilityValue u = uc.calcUtility(P1, P1, 1);
	EXPECT_LE(u, -SMALL_INF);
}

TEST_F(UtilityCalcFixture, testOneCaptureWorthMoreThanAThree) {
	setLineCounts(P1, 0,0,0,0,0);
	setLineCounts(P2, 0,0,1,0,0);
	setCaptured(2, 0);

	UtilityValue u = uc.calcUtility(P1, P1, 1);
	EXPECT_GT(u, 0);
}

TEST_F(UtilityCalcFixture, testOneCaptureWorthLessThanAFour) {
	setLineCounts(P1, 0,0,0,0,0);
	setLineCounts(P2, 0,0,0,1,0);
	setCaptured(2, 0);

	UtilityValue u = uc.calcUtility(P2, P1, 1);
	EXPECT_LE(u, 0);
}

TEST_F(UtilityCalcFixture, testP2Search) {
	// Search by P2
	setLineCounts(P1, 0,0,0,0,0);
	setLineCounts(P2, 0,0,1,0,0);
	UtilityValue u = uc.calcUtility(P1, P2, 1);
	EXPECT_GE(u, 0);
}

TEST_F(UtilityCalcFixture, testP2CaptureSearch) {
	// Search by P2 after capture 
	setCaptured(0, 2);
	UtilityValue u = uc.calcUtility(P1, P2, 1);
	EXPECT_GE(u, 0);
}

TEST_F(UtilityCalcFixture, testP1ToMoveAdvantageAgainstP2Search) {
	setLineCounts(P1, 1,0,0,0,0);
	setLineCounts(P2, 1,0,0,0,0);
	UtilityValue u = uc.calcUtility(P1, P2, 1);
	EXPECT_LE(u, 0);
}

TEST_F(UtilityCalcFixture, testWhiteHavingTheMoveGetsAHigherUtil) {
	setLineCounts(P1, 1,0,0,0,0);
	setLineCounts(P2, 2,0,0,0,0);

	UtilityValue uWithMove = uc.calcUtility(P2, P2, 1);
	UtilityValue uNotToMove = uc.calcUtility(P1, P2, 1);

	EXPECT_GT(uWithMove, uNotToMove);
}

TEST_F(UtilityCalcFixture, testBlackHavingTheMoveGetsAHigherUtil) {
	// Search by black 
	setLineCounts(P1, 1,0,0,0,0);
	setLineCounts(P2, 2,0,0,0,0);

	UtilityValue uWithMove = uc.calcUtility(P1, P1, 1);
	UtilityValue uNotToMove = uc.calcUtility(P2, P1, 1);

	EXPECT_GT(uWithMove, uNotToMove);
}

TEST_F(UtilityCalcFixture, testNextToMiddleIsBetter) {
	// Search by P2
	// (-783, [16, 0, 0, 0, 0][11, 0, 0, 0, 0] - (3, 3) next to 4,4
	setLineCounts(P1, 16,0,0,0,0);
	setLineCounts(P2, 11,0,0,0,0);
	UtilityValue uAdjacent = uc.calcUtility(P1, P2, 1);

	// (-588, [17, 0, 0, 0, 0][7, 0, 0, 0, 0] - (6, 6) with a gap
	setLineCounts(P1, 17,0,0,0,0);
	setLineCounts(P2, 7,0,0,0,0);
	UtilityValue uDist = uc.calcUtility(P1, P2, 1);

	EXPECT_GT(uAdjacent, uDist);
}

TEST_F(UtilityCalcFixture, testOneTakeIsWorthMoreThanAFewPairs) {
	setLineCounts(P1, 0,0,0,0,0);
	setLineCounts(P2, 11,3,0,0,0);
	setTakes(1, 0);

	UtilityValue u = uc.calcUtility(P1, P1, 1);
	EXPECT_GT(u, 0);
}

TEST_F(UtilityCalcFixture, testOneTakeIsWorthMoreThanTwoThrees) {
	// I'm still not sure about this one
	setLineCounts(P1, 0,0,0,0,0);
	setLineCounts(P2, 0,0,2,0,0);
	setTakes(1, 0);

	UtilityValue u = uc.calcUtility(P1, P1, 1);
	EXPECT_GT(u, 0);
}

TEST_F(UtilityCalcFixture, testOneTakeWithTheMoveIsWorthMoreThanOneThree) {
	setLineCounts(P1, 0,0,0,0,0);
	setLineCounts(P2, 0,0,1,0,0);
	setTakes(1, 0);

	UtilityValue u = uc.calcUtility(P1, P1, 1);
	EXPECT_GT(u, 0);
}

TEST_F(UtilityCalcFixture, testOneThreeWithTheMoveIsWorthMoreThanOneTake) {
	setLineCounts(P1, 0,0,0,0,0);
	setLineCounts(P2, 0,0,1,0,0);
	setTakes(1, 0);

	UtilityValue u = uc.calcUtility(P2, P1, 1);
	EXPECT_LT(u, 0);
}

TEST_F(UtilityCalcFixture, testFourCapturesWorthMoreThan3Threes) {
	setLineCounts(P1, 0,0,0,0,0);
	setLineCounts(P2, 0,0,3,0,0);
	setCaptured(8, 0);

	UtilityValue u = uc.calcUtility(P1, P1, 1);
	EXPECT_GT(u, 0);
}

TEST_F(UtilityCalcFixture, testFourInARowWithTheMoveIsAWin) {
	setLineCounts(P1, 0,0,0,1,0);
	setLineCounts(P2, 0,0,0,0,0);

	UtilityValue u = uc.calcUtility(P1, P1, 1);
	EXPECT_GT(u, SMALL_INF);
}

TEST_F(UtilityCalcFixture, testFourInARowWithoutTheMoveIsNotWon) {
	setLineCounts(P1, 0,0,0,1,0);
	setLineCounts(P2, 0,0,0,0,0);

	UtilityValue u = uc.calcUtility(P2, P1, 1);
	EXPECT_LT(u, SMALL_INF);
}

TEST_F(UtilityCalcFixture, testFourInARowForOppositionWithTheMoveIsALoss) {
	setLineCounts(P1, 0,0,0,0,0);
	setLineCounts(P2, 0,0,0,1,0);

	UtilityValue u = uc.calcUtility(P2, P1, 1);
	EXPECT_LT(u, -SMALL_INF);
}

TEST_F(UtilityCalcFixture, testFourCapturesAndAThreatWithTheMoveIsAWin) {
	// setSearchPlayerColour(P1)
	// setTurnPlayerColour(P1)

	setCaptured(8, 0);
	setTakes(1, 0);
	UtilityValue u = uc.calcUtility(P1, P1, 1);
	EXPECT_GT(u, SMALL_INF);
}

TEST_F(UtilityCalcFixture, testFourCapturesWithNoThreatsWithTheMoveIsNotAWin) {
	// setSearchPlayerColour(P1)
	// setTurnPlayerColour(P1)

	setCaptured(8, 0);
	setTakes(0, 0);
	UtilityValue u = uc.calcUtility(P1, P1, 1);
	EXPECT_LT(u, SMALL_INF);
}

TEST_F(UtilityCalcFixture, testFourCapturesAndAThreatForOppenentWithTheMoveIsALoss) {
	// setSearchPlayerColour(P1)
	// setTurnPlayerColour(P2)

	setCaptured(0, 8);
	setTakes(0, 1);
	UtilityValue u = uc.calcUtility(P2, P1, 1);
	EXPECT_LT(u, -SMALL_INF);
}

TEST_F(UtilityCalcFixture, testThreeCapturesAndAThreatForOppenentWithTheMoveIsNotALoss) {
	// setSearchPlayerColour(P1)
	// setTurnPlayerColour(P2)

	setCaptured(0, 6);
	setTakes(0, 1);
	UtilityValue u = uc.calcUtility(P2, P1, 1);
	EXPECT_GT(u, -SMALL_INF);
}

TEST_F(UtilityCalcFixture, testTakeHasAValue) {
	// setSearchPlayerColour(P1)
	// setTurnPlayerColour(P2)

	setTakes(1, 0);
	UtilityValue u = uc.calcUtility(P2, P1, 1);
	EXPECT_GT(u, 0);
}

TEST_F(UtilityCalcFixture, testThreatHasAValue) {
	// setSearchPlayerColour(P1)
	// setTurnPlayerColour(P2)

	setThreats(1, 0);
	UtilityValue u = uc.calcUtility(P2, P1, 1);
	EXPECT_GT(u, 0);
}

#if 0

TEST_F(UtilityCalcFixture, testTwoFoursWithNoDangerOfBeingCapturedIsAWin) {
	// setSearchPlayerColour(P1)
	// setTurnPlayerColour(P2)

	setLineCounts(P1, 0,0,0,2,0);
	setTakes(0, 0)
	UtilityValue u = uc.calcUtility(P2, P1, 1);
	EXPECT_GT(u, SMALL_INF);
}

TEST_F(UtilityCalcFixture, testFourPairsCapturedAndThreeTakesWillWin) {
	// setSearchPlayerColour(P1)
	// setTurnPlayerColour(P2)

	setCaptured(8, 0);
	setTakes(3, 0)
	UtilityValue u = uc.calcUtility(P2, P1, 1);
	EXPECT_GT(u, SMALL_INF);
}

TEST_F(UtilityCalcFixture, atestFourPairsCapturedAndThreeTakesWillWin) {
	// setSearchPlayerColour(P1)
	// setTurnPlayerColour(P2)

	setLineCounts(P1, 0,0,3,0,0);
	setLineCounts(P2, 0,0,0,0,0);
	setCaptured(0, 2);
	setTakes(0, 0)

	UtilityValue u = uc.calcUtility(P2, P1, 1);
	EXPECT_GT(u, SMALL_INF);
}

######################################################

TEST_F(UtilityCalcFixture, testTrickyPos1) {
	// setSearchPlayerColour(P1)
	// setTurnPlayerColour(P2)

	setCaptured(4, 4);
	setTakes(0, 0)
	setThreats(0, 0);
	setLineCounts(P1, 78, 9, 1, 1, 0);
	setLineCounts(P2, 36, 2, 0, 0, 0);
	UtilityValue u = uc.calcUtility(P2, P1, 1);

	setCaptured(0, 0);
	setTakes(0, 0)
	setThreats(2, 2);
	setLineCounts(P1, 51, 8, 0, 0, 0);
	setLineCounts(P2, 28, 3, 1, 0, 0);
	UtilityValue u = uc.calcUtility(P2, P1, 1);

	EXPECT_GT(u1, u2);
}

TEST_F(UtilityCalcFixture, testTrickyPos2) {
	// setSearchPlayerColour(P1)
	// setTurnPlayerColour(P1)

	setCaptured(0, 0);
	setTakes(0, 1)
	setThreats(0, 0);
	setLineCounts(P1, 34, 5, 1, 0, 0);
	setLineCounts(P2, 49, 6, 0, 0, 0);
	UtilityValue u = uc.calcUtility(P2, P1, 1);

	setCaptured(2, 2);
	setTakes(0, 0)
	setThreats(2, 0);
	setLineCounts(P1, 49, 4, 0, 0, 0);
	setLineCounts(P2, 48, 5, 1, 0, 0);
	UtilityValue u = uc.calcUtility(P2, P1, 1);

	EXPECT_GT(u1, u2);
}

TEST_F(UtilityCalcFixture, testStrange) {
	// setSearchPlayerColour(P2)
	// setTurnPlayerColour(P2)

	setCaptured(2, 2);
	setTakes(0, 0)
	setThreats(0, 0);
	setLineCounts(P1, 29, 2, 0, 0, 0);
	setLineCounts(P2, 33, 1, 0, 0, 0);
	UtilityValue u = uc.calcUtility(P2, P1, 1);

	setCaptured(2, 0);
	setTakes(0, 1)
	setThreats(0, 0);
	setLineCounts(P1, 53, 3, 0, 0, 0);
	setLineCounts(P2, 24, 3, 0, 0, 0);
	UtilityValue u = uc.calcUtility(P2, P1, 1);

	EXPECT_GT(u1, u2);
}

TEST_F(UtilityCalcFixture, atestTrickyPos2b) {
	# TODO
	// setSearchPlayerColour(P1)
	// setTurnPlayerColour(P1)

	setCaptured(2, 2);
	setTakes(0, 0)
	setThreats(0, 0);
	setLineCounts(P1, 59, 4, 0, 0, 0);
	setLineCounts(P2, 61, 3, 1, 0, 0);
	UtilityValue u = uc.calcUtility(P2, P1, 1);

	setCaptured(2, 2);
	setTakes(0, 0)
	setThreats(2, 0);
	setLineCounts(P1, 49, 4, 0, 0, 0);
	setLineCounts(P2, 48, 5, 1, 0, 0);
	UtilityValue u = uc.calcUtility(P2, P1, 1);

	EXPECT_GT(u1, u2);
}

TEST_F(UtilityCalcFixture, testYetAnother) {
	// setSearchPlayerColour(P1)
	// setTurnPlayerColour(P1)

	'''
This should have got the highest score
((-55692, 0), ((9, 6), 14. Lines: [None, [38, 5, 0, 0, 0], [29, 0, 0, 0, 0]], Takes: [0, 0, 0], Threats: [0, 0, 2], Best: [{}, {}, {}] Captures: [0, 4, 4]))

But these all scored much higher
((33504, 0), ((5, 8), 14. Lines: [None, [35, 4, 0, 0, 0], [52, 1, 0, 0, 0]], Takes: [0, 1, 0], Threats: [0, 0, 2], Best: [{}, {}, {}] Captures: [0, 2, 4]))
((35313, 0), ((2, 2), 14. Lines: [None, [30, 4, 1, 0, 0], [56, 3, 0, 0, 0]], Takes: [0, 1, 0], Threats: [0, 0, 2], Best: [{}, {}, {}] Captures: [0, 2, 4]))
((36080, 0), ((9, 9), 14. Lines: [None, [35, 4, 1, 0, 0], [56, 3, 0, 0, 0]], Takes: [0, 1, 0], Threats: [0, 0, 2], Best: [{}, {}, {}] Captures: [0, 2, 4]))
((82186, 0), ((6, 7), 14. Lines: [None, [26, 8, 0, 0, 0], [49, 1, 0, 0, 0]], Takes: [0, 1, 0], Threats: [0, 0, 4], Best: [{}, {}, {}] Captures: [0, 2, 4]))

Utility for 14: (-22938, 0) (Lines: [None, [26, 8, 0, 0, 0], [49, 1, 0, 0, 0]], Takes: [0, 1, 0], Threats: [0, 0, 4], Best: NullFilter)
	'''
	self.setCaptured(4, 4);
	self.setTakes(0, 0)
	self.setThreats(0, 2);
	self.setMoveNumber(14)
	self.setLineCounts(P1, 38, 5, 0, 0, 0);
	self.setLineCounts(P2, 29, 0, 0, 0, 0);
	UtilityValue u = uc.calcUtility(P2, P1, 1);

	self.setCaptured(2, 4);
	self.setTakes(1, 0)
	self.setThreats(0, 4);
	self.setLineCounts(P1, 26, 8, 0, 0, 0);
	self.setLineCounts(P2, 49, 1, 0, 0, 0);
	UtilityValue u = uc.calcUtility(P2, P1, 1);

	self.EXPECT_GT(u1, u2);
}

if Name == "Main":
    unittest.main()


#!/usr/bin/env python

import unittest

#from pentai.base.player import *
#import pentai.base.gameState
import pentai.base.board as bM
from pentai.base.mock import *

from pentai.ai.lengthLookupTable import *
import pentai.ai.abState as absM
import pentai.ai.utilityCalculator as ucM
import pentai.ai.utilityStats as usM
import pentai.ai.priorityFilter as pfM # TODO: NullFilter
import pentai.ai.aiGenome as aigM
import pentai.db.aiFactory as aifM # Hmmm. Shouldn't need to use this here

import itertools

inf = INFINITY / 1000

class UtilityTest(unittest.TestCase):
    def setUp(self):

        self.searchFilter = pfM.PriorityFilter()
        self.utilCalc = ucM.UtilityCalculator()

        # Set defaults for utility calculation
        player = Mock({"getUtilityCalculator":self.utilCalc})
        genome = aigM.AIGenome("Irrelevant")
        aif = aifM.AIFactory()
        aif.setUtilityConfig(genome, player)

        self.s = absM.ABState(searchFilter=self.searchFilter,
                utilityCalculator=self.utilCalc)
        self.us = usM.UtilityStats()
        self.rules = Mock()
        self.rules.stonesForCaptureWin = 10
        self.rules.canCapturePairs = True
        self.game = Mock()
        self.captured = [0, 0, 0] # This is individual stones, E/B/W
        self.gs = Mock({"getAllCaptured": self.captured,
            "getMoveNumber": 10,
            "game":self.game,
            "getWonBy": EMPTY,
            "getRules":self.rules}) 
        self.gs.board = bM.Board(13)
        self.gs.game = self.game
        // self.setTurnPlayerColour(P1)
        // self.setSearchPlayerColour(P1)
        self.s.setState(self.gs)

    def utility(self):
        util = self.s.utility()
        return util

    def setLines(self, pn, lines):
        us = self.s.utilityStats
        us.lines[pn] = lines

    def setBlackLines(self, lines):
        self.setLines(P1, lines)

    def setWhiteLines(self, lines):
        self.setLines(P2, lines)

    def setTakes(self, blackTakes, whiteTakes):
        self.s.utilityStats.takes = [0, blackTakes, whiteTakes]

    def setThreats(self, blackThreats, whiteThreats):
        self.s.utilityStats.threats = [0, blackThreats, whiteThreats]

    def setCaptured(self, blackCaptures, whiteCaptures):
        self.captured[P1] = blackCaptures
        self.captured[P2] = whiteCaptures

    def setTurnPlayerColour(self, turnPlayerColour):
        // Set whose move it is at the leaf state 
        self.gs.mockAddReturnValues(toMoveColour=turnPlayerColour)
        
    def setSearchPlayerColour(self, searchPlayerColour):
        // Set whose move it is at the leaf state 
        self.game.mockAddReturnValues(toMoveColour=searchPlayerColour)

    def setMoveNumber(self, mn):
        self.gs.mockAddReturnValues(getMoveNumber=mn)

TEST_F(UtilityCalcFixture, testBlackNoWinByCapturesForFiveInARow) {
	setLineCounts(P1, 0,0,0,0,0);
	setLineCounts(P2, 0,0,0,0,0);
	setCaptured(10, 0)
	rules.stonesForCaptureWin = 0
	u = utility()
	assertEqual(u, 0)

TEST_F(UtilityCalcFixture, testWhiteNoWinByCapturesForFiveInARow) {
	setLineCounts(P1, 0,0,0,0,0);
	setLineCounts(P2, 0,0,0,0,0);
	setCaptured(0, 10)
	rules.stonesForCaptureWin = 0
	u = utility()
	assertEqual(u, 0)

TEST_F(UtilityCalcFixture, atestOneTakeIsWorthLessThanThreeThrees) {
	// setSearchPlayerColour(P1)
	// setTurnPlayerColour(P1)

	setLineCounts(P1, 0,0,0,0,0);
	setLineCounts(P2, 0,0,3,0,0);
	setTakes(1, 0);
	u = utility();
	EXPECT_LT(u, 0);
}

#endif

