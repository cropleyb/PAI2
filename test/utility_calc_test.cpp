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
	//
	CapCount getCaptured(Colour c) const
	{
		return _captured[c];
	}

	const PattCount *getPatternCounts(Colour c) const
	{
		return _patternCounts[c];
	}
	
	UtilityValue getCheckerboardContrib(Colour /*c*/) const
	{
		// TODO: test this...
		return 0;
	}

	UtilityValue getStripeContrib(Colour /*c*/) const
	{
		// TODO: test this...
		return 0;
	}

	bool getCanWinByCaptures() const { return true; }
	
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
	setLineCounts(P1, 0,0,0,0,0);
	setLineCounts(P2, 0,0,2,0,0);
	setTakes(1, 0);

	UtilityValue u = uc.calcUtility(P1, P1, 1);
	EXPECT_GT(u, 0);
}

TEST_F(UtilityCalcFixture, testOneTakeIsWorthLessThanThreeThrees) {
	setLineCounts(P1, 0,0,0,0,0);
	setLineCounts(P2, 0,0,3,0,0);
	setTakes(1, 0);

	UtilityValue u = uc.calcUtility(P1, P1, 14);
	EXPECT_LT(u, 0);
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
	setCaptured(8, 0);
	setTakes(1, 0);

	UtilityValue u = uc.calcUtility(P1, P1, 1);
	EXPECT_GT(u, SMALL_INF);
}

TEST_F(UtilityCalcFixture, testFourCapturesWithNoThreatsWithTheMoveIsNotAWin) {
	setCaptured(8, 0);
	setTakes(0, 0);

	UtilityValue u = uc.calcUtility(P1, P1, 1);
	EXPECT_LT(u, SMALL_INF);
}

TEST_F(UtilityCalcFixture, testFourCapturesAndAThreatForOppenentWithTheMoveIsALoss) {
	setCaptured(0, 8);
	setTakes(0, 1);

	UtilityValue u = uc.calcUtility(P2, P1, 1);
	EXPECT_LT(u, -SMALL_INF);
}

TEST_F(UtilityCalcFixture, testThreeCapturesAndAThreatForOppenentWithTheMoveIsNotALoss) {
	setCaptured(0, 6);
	setTakes(0, 1);

	UtilityValue u = uc.calcUtility(P2, P1, 1);
	EXPECT_GT(u, -SMALL_INF);
}

TEST_F(UtilityCalcFixture, testTakeHasAValue) {
	setTakes(1, 0);

	UtilityValue u = uc.calcUtility(P2, P1, 1);
	EXPECT_GT(u, 0);
}

TEST_F(UtilityCalcFixture, testThreatHasAValue) {
	setThreats(1, 0);

	UtilityValue u = uc.calcUtility(P2, P1, 1);
	EXPECT_GT(u, 0);
}

TEST_F(UtilityCalcFixture, testTwoFoursWithNoDangerOfBeingCapturedIsAWin) {
	// setTurnPlayerColour(P2)

	setLineCounts(P1, 0,0,0,2,0);
	setTakes(0, 0);
	UtilityValue u = uc.calcUtility(P2, P1, 1);
	EXPECT_GT(u, SMALL_INF);
}

TEST_F(UtilityCalcFixture, testFourPairsCapturedAndThreeTakesWillWin) {
	setCaptured(8, 0);
	setTakes(3, 0);
	UtilityValue u = uc.calcUtility(P2, P1, 1);
	EXPECT_GT(u, SMALL_INF);
}

//######################################################

TEST_F(UtilityCalcFixture, testTrickyPos1) {
	setCaptured(4, 4);
	setTakes(0, 0);
	setThreats(0, 0);
	setLineCounts(P1, 78, 9, 1, 1, 0);
	setLineCounts(P2, 36, 2, 0, 0, 0);
	UtilityValue u1 = uc.calcUtility(P2, P1, 1);

	setCaptured(0, 0);
	setTakes(0, 0);
	setThreats(2, 2);
	setLineCounts(P1, 51, 8, 0, 0, 0);
	setLineCounts(P2, 28, 3, 1, 0, 0);
	UtilityValue u2 = uc.calcUtility(P2, P1, 1);

	EXPECT_GT(u1, u2);
}

TEST_F(UtilityCalcFixture, testTrickyPos2) {
	setCaptured(0, 0);
	setTakes(0, 1);
	setThreats(0, 0);
	setLineCounts(P1, 34, 5, 1, 0, 0);
	setLineCounts(P2, 49, 6, 0, 0, 0);
	UtilityValue u1 = uc.calcUtility(P2, P1, 1);

	setCaptured(2, 2);
	setTakes(0, 0);
	setThreats(2, 0);
	setLineCounts(P1, 49, 4, 0, 0, 0);
	setLineCounts(P2, 48, 5, 1, 0, 0);
	UtilityValue u2 = uc.calcUtility(P2, P1, 1);

	EXPECT_GT(u1, u2);
}

TEST_F(UtilityCalcFixture, testStrange) {
	// setSearchPlayerColour(P2)
	// setTurnPlayerColour(P2)

	setCaptured(2, 2);
	setTakes(0, 0);
	setThreats(0, 0);
	setLineCounts(P1, 29, 2, 0, 0, 0);
	setLineCounts(P2, 33, 1, 0, 0, 0);
	UtilityValue u1 = uc.calcUtility(P2, P2, 1);

	setCaptured(2, 0);
	setTakes(0, 1);
	setThreats(0, 0);
	setLineCounts(P1, 53, 3, 0, 0, 0);
	setLineCounts(P2, 24, 3, 0, 0, 0);
	UtilityValue u2 = uc.calcUtility(P2, P2, 1);

	EXPECT_GT(u1, u2);
}

#if 0
TEST_F(UtilityCalcFixture, atestTrickyPos2b) {
	// TODO
	// setSearchPlayerColour(P1)
	// setTurnPlayerColour(P1)

	setCaptured(2, 2);
	setTakes(0, 0);
	setThreats(0, 0);
	setLineCounts(P1, 59, 4, 0, 0, 0);
	setLineCounts(P2, 61, 3, 1, 0, 0);
	UtilityValue u1 = uc.calcUtility(P1, P1, 1);

	setCaptured(2, 2);
	setTakes(0, 0);
	setThreats(2, 0);
	setLineCounts(P1, 49, 4, 0, 0, 0);
	setLineCounts(P2, 48, 5, 1, 0, 0);
	UtilityValue u2 = uc.calcUtility(P1, P1, 1);

	EXPECT_GT(u1, u2);
}
#endif

TEST_F(UtilityCalcFixture, testYetAnother) {
	/*
This should have got the highest score
((-55692, 0), ((9, 6), 14. Lines: [None, [38, 5, 0, 0, 0], [29, 0, 0, 0, 0]], Takes: [0, 0, 0], Threats: [0, 0, 2], Best: [{}, {}, {}] Captures: [0, 4, 4]))

But these all scored much higher
((33504, 0), ((5, 8), 14. Lines: [None, [35, 4, 0, 0, 0], [52, 1, 0, 0, 0]], Takes: [0, 1, 0], Threats: [0, 0, 2], Best: [{}, {}, {}] Captures: [0, 2, 4]))
((35313, 0), ((2, 2), 14. Lines: [None, [30, 4, 1, 0, 0], [56, 3, 0, 0, 0]], Takes: [0, 1, 0], Threats: [0, 0, 2], Best: [{}, {}, {}] Captures: [0, 2, 4]))
((36080, 0), ((9, 9), 14. Lines: [None, [35, 4, 1, 0, 0], [56, 3, 0, 0, 0]], Takes: [0, 1, 0], Threats: [0, 0, 2], Best: [{}, {}, {}] Captures: [0, 2, 4]))
((82186, 0), ((6, 7), 14. Lines: [None, [26, 8, 0, 0, 0], [49, 1, 0, 0, 0]], Takes: [0, 1, 0], Threats: [0, 0, 4], Best: [{}, {}, {}] Captures: [0, 2, 4]))

Utility for 14: (-22938, 0) (Lines: [None, [26, 8, 0, 0, 0], [49, 1, 0, 0, 0]], Takes: [0, 1, 0], Threats: [0, 0, 4], Best: NullFilter)
	*/
	setCaptured(4, 4);
	setTakes(0, 0);
	setThreats(0, 2);
	setLineCounts(P1, 38, 5, 0, 0, 0);
	setLineCounts(P2, 29, 0, 0, 0, 0);
	UtilityValue u1 = uc.calcUtility(P1, P1, 14);

	setCaptured(2, 4);
	setTakes(1, 0);
	setThreats(0, 4);
	setLineCounts(P1, 26, 8, 0, 0, 0);
	setLineCounts(P2, 49, 1, 0, 0, 0);
	UtilityValue u2 = uc.calcUtility(P1, P1, 14);

	EXPECT_GT(u1, u2);
}


TEST_F(UtilityCalcFixture, testLineValue) {
	UtilityValue u;

	setLineCounts(P1, 1, 0, 0, 0, 0);
	u = uc.calcUtility(P1, P1, 1);
	EXPECT_EQ(45, u);

	setLineCounts(P1, 0, 1, 0, 0, 0);
	u = uc.calcUtility(P1, P1, 1);
	EXPECT_EQ(1575, u);

	setLineCounts(P1, 0, 0, 1, 0, 0);
	u = uc.calcUtility(P1, P1, 1);
	EXPECT_EQ(55125, u);

	setLineCounts(P1, 0, 0, 0, 1, 0);
	u = uc.calcUtility(P1, P1, 1);
	EXPECT_LE(1e+20, u);
}

TEST_F(UtilityCalcFixture, testCapValue) {
	UtilityValue u;

	setCaptured(2, 0);
	u = uc.calcUtility(P1, P1, 1);
	EXPECT_EQ(27000, u);

	setCaptured(4, 0);
	u = uc.calcUtility(P1, P1, 1);
	EXPECT_EQ(54000, u);

	setCaptured(6, 0);
	u = uc.calcUtility(P1, P1, 1);
	EXPECT_EQ(162000, u);

	setCaptured(8, 0);
	u = uc.calcUtility(P1, P1, 1);
	EXPECT_EQ(432000, u);

	setCaptured(10, 0);
	u = uc.calcUtility(P1, P1, 1);
	EXPECT_LE(1e+20, u);
}

TEST_F(UtilityCalcFixture, testTakeValue) {
	UtilityValue u;

	setTakes(1, 0);
	u = uc.calcUtility(P1, P1, 1);
	EXPECT_EQ(3600, u);

	setTakes(2, 0);
	u = uc.calcUtility(P1, P1, 1);
	EXPECT_EQ(7200, u);

	setTakes(4, 0);
	u = uc.calcUtility(P1, P1, 1);
	EXPECT_EQ(14400, u);

	setTakes(6, 0);
	u = uc.calcUtility(P1, P1, 1);
	EXPECT_EQ(21600, u);

	setTakes(8, 0);
	u = uc.calcUtility(P1, P1, 1);
	EXPECT_EQ(28800, u);
	
	setTakes(10, 0);
	u = uc.calcUtility(P1, P1, 1);
	EXPECT_LE(36000, u);
}

TEST_F(UtilityCalcFixture, testThreatValue) {
	UtilityValue u;

	setThreats(1, 0);
	u = uc.calcUtility(P1, P1, 1);
	EXPECT_EQ(900, u);

	setThreats(2, 0);
	u = uc.calcUtility(P1, P1, 1);
	EXPECT_EQ(1800, u);

	setThreats(4, 0);
	u = uc.calcUtility(P1, P1, 1);
	EXPECT_EQ(3600, u);

	setThreats(6, 0);
	u = uc.calcUtility(P1, P1, 1);
	EXPECT_EQ(5400, u);

	setThreats(8, 0);
	u = uc.calcUtility(P1, P1, 1);
	EXPECT_EQ(7200, u);

	setThreats(10, 0);
	u = uc.calcUtility(P1, P1, 1);
	EXPECT_LE(9000, u);
}

#if 0

TEST_F(UtilityCalcFixture, testBlackNoWinByCapturesForFiveInARow) {
	setLineCounts(P1, 0,0,0,0,0);
	setLineCounts(P2, 0,0,0,0,0);
	setCaptured(10, 0)
	rules.stonesForCaptureWin = 0
	UtilityValue u = uc.calcUtility(P1, P1, 14);
	assertEqual(u, 0)

TEST_F(UtilityCalcFixture, testWhiteNoWinByCapturesForFiveInARow) {
	setLineCounts(P1, 0,0,0,0,0);
	setLineCounts(P2, 0,0,0,0,0);
	setCaptured(0, 10)
	rules.stonesForCaptureWin = 0
	UtilityValue u = uc.calcUtility(P1, P1, 14);
	assertEqual(u, 0)

#endif


