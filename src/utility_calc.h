#ifndef _utility_calc_h
#define _utility_calc_h

#include "defines.h"
#include "line_pattern.h"

#include <array>

using namespace std;

typedef double UtilityValue;

template <class PS>
class UtilityCalc
{
public:
	UtilityCalc(const PS &ps) : _posStats(ps),
        _capturesScale {1, 1, 1, 2, 4, 8},
        _patternScale {1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1} // TODO: Cull
	{
		// TEMP until we have an equiv. to ai_genome
		_moveFactor = 95.0;
        //_captureScoreBase = 300;
        _captureScoreBase = 600; // Doubled because it is per pair
        //_captureScoreBase = 200;
        _takeScoreBase = 80;
        _threatScoreBase = 20;
        //_blockedFourBase = 200; // Halved as only one is blocked
        _blockedFourBase = 0;
        _takeBlocked4ScoreBase = 500; // Guesses for take targeting
        _takeTakeScoreBase = 500;
        _takeFourScoreBase = 500;
        _lengthFactor = 35;
        _judgement = 100;
        //_checkerboardValue = 35;
        _checkerboardValue = 400;
        //_stripeValue = 0;
        _stripeValue = 400;
        //_spreadValue = 0;
		_multiFactor = 0.0;
	}

	void setPAI1()
	{
		_moveFactor = 45.0;
		_captureScoreBase = 600; // Doubled because it is per pair
		_takeScoreBase = 80;
		_threatScoreBase = 20;
		_blockedFourBase = 400;
		_lengthFactor = 35;
		_checkerboardValue = 35;
		_takeBlocked4ScoreBase = 0;
		_takeTakeScoreBase = 0;
		_takeFourScoreBase = 0;
		_stripeValue = 0;
		_spreadValue = 0;
		_multiFactor = 0.0;
		_sobCapFactor = 0.0;
		_sobSqrCapFactor = 0.0;
	}

	void setPartPAI2()
	{
		// TEMP until we have an equiv. to ai_genome
		_moveFactor = 45.0;
        _captureScoreBase = 600; // Doubled because it is per pair
        _takeScoreBase = 80;
        _threatScoreBase = 20;
		//_sobCapFactor = 0.005; 29/27
		//_sobCapFactor = 0.003; 23/33
		//_sobCapFactor = 0.001; 28/28
		//_sobCapFactor = 0.0005; 27/29
		//_sobSqrCapFactor = 0.0001; 26/30
		//_sobSqrCapFactor = 0.00003; 29/27
		//_sobSqrCapFactor = 0.00001; 26/30
        //_spreadValue = 30; // Better for Standard, worse for Tournament
        //_stripeValue = 15; // Better for Standard, same for Tournament
		//_multiFactor = 0.3;
        //_blockedFourBase = 200; // Halved as only one is blocked
        //_takeTakeScoreBase = 100;
        //_takeBlocked4ScoreBase = 100; // Guesses for take targeting
        //_takeFourScoreBase = 20;
#if 0
        _blockedFourBase = 0;
        _lengthFactor = 35;
        //_checkerboardValue = 35;
        _checkerboardValue = 400;
        //_stripeValue = 0;
        //_spreadValue = 0;
        _spreadValue = 0;
#endif
	}

	UtilityValue calcUtility(Colour turnColour, Colour searchColour, MoveNumber moveNumber) const;

private:
	UtilityValue utilityScore(Colour evalColour, Colour turnColour) const;

	bool zeroTurnWin(Colour evalColour, Colour turnColour) const;
	bool oneTurnWin(Colour evalColour, Colour turnColour) const;

	UtilityValue capturedContrib(CapCount captures) const;
	UtilityValue takeContrib(CapCount takes, CapCount captures) const;
	UtilityValue threatContrib(CapCount threats, CapCount captures) const;

	const PS &_posStats;

	double _moveFactor;
	double _patternScale[MAX_PATTERN_TYPE];
	int _captureScoreBase;
	int _takeScoreBase;
	int _threatScoreBase;
	int _blockedFourBase;
	int _takeBlocked4ScoreBase;
	int _takeTakeScoreBase;
	int _takeFourScoreBase;
	std::array<int,6> _capturesScale;
	// _lengthScale;
	int _lengthFactor;
	int _judgement;
	int _checkerboardValue;
	int _stripeValue;
	int _spreadValue;
	double _multiFactor;
	double _sobCapFactor;
	double _sobSqrCapFactor;
};

template <class PS>
UtilityValue UtilityCalc<PS>::calcUtility(Colour turnColour, Colour searchColour, MoveNumber moveNumber) const
{
	// The searchColour is the colour of the AI player doing the search.
	// The turnColour is the colour of the player to move at the leaf
	// state of the search.
	// evalColour is the colour that we are assessing
	// the utility value for - we do both for each call to utility
	Colour otherColour = otherPlayer(turnColour);
	Colour bothColours[2] = {turnColour, otherColour};

	// Check for immediate wins first, then forceable wins
	for (int ttw=0; ttw<=1; ttw++)
	{
		for (int cInd=0; cInd<=1; cInd++)
		{
			Colour evalColour = bothColours[cInd];

			bool won;
			if (ttw==0) won = zeroTurnWin(evalColour, turnColour);
			else won = oneTurnWin(evalColour, turnColour);

			if (won)
			{
				// Scale these INFINITIES down to discourage sadistic
				// won game lengthening.
				if (searchColour == evalColour)
				{
					// If the winner is us then / by move number.
					return BIG_NUM * 100.0 / (moveNumber*moveNumber);
					// TODO: Sadistic mode for Rich
					// - multiply by move number ;)
				} else {
					// If the winner is not us then also / by moveNumber
					return -BIG_NUM * 100.0 / (moveNumber*moveNumber);
				}
			}
		}
	}

	// No forceable win has been found, so fudge up a score
	UtilityValue utilScores[3] = {0.0, 0.0, 0.0};

	for (int evalColour=P1; evalColour<=P2; evalColour++) // in (turn_colour, other_colour):
	{
		UtilityValue util = utilityScore(evalColour, turnColour);
		utilScores[evalColour] = util;
	}

	// It is a very significant advantage to have the move
	utilScores[turnColour] *= _moveFactor;

	UtilityValue ourScore = utilScores[turnColour];
	UtilityValue theirScore = utilScores[otherColour];

	UtilityValue ret = ourScore - theirScore;

	if (searchColour != turnColour)
		ret = theirScore - ourScore;

	//cout << "our score: " << ourScore << "; theirScore: " << theirScore << 
	//	"; ret: " << ret << endl;

	return ret;
}

template <class PS>
UtilityValue UtilityCalc<PS>::utilityScore(Colour evalColour, Colour /*turnColour*/) const
{
	Colour evalCaptured = _posStats.getCaptured(evalColour);
	Colour otherColour = otherPlayer(evalColour);
	Colour otherCaptured = _posStats.getCaptured(otherColour);
	
	CapCount captured = evalCaptured;

	UtilityValue score = 0.0;

	const PattCount *evalPatterns = _posStats.getPatternCounts(evalColour);

	for (int i=Line4; i>0; i-=1)
	{
		if (i == Take || i == Threat) continue;
		score *= _lengthFactor;
		double contrib = evalPatterns[i] * _patternScale[i];
		if (_multiFactor > 0 and i >= Line3) {
			Breadth multis = _posStats.getMultiCount(evalColour, i);
			contrib *= (multis + 1) * _multiFactor;
		}
		score += contrib;
	}
    bool takeTargeting = _posStats._takeTargeting;
    if (takeTargeting) {
        score += evalPatterns[TakeTake] * _takeTakeScoreBase;
        score += evalPatterns[FourTake] * _takeFourScoreBase;
        score += evalPatterns[Blocked4Take] * _takeBlocked4ScoreBase;
    }

	// Unless we're playing keryo, capturesScale only needs to operate
	// on pairs
	captured /= 2;

	if (_posStats.getCanWinByCaptures())
	{
		// stones on the board
		float sob = _posStats.getStonesOnBoard(evalColour);
		float sobFactor = sob*sob*_sobSqrCapFactor + sob*_sobCapFactor + 1;
		UtilityValue cc = sobFactor * capturedContrib(captured);
		score += cc;

		UtilityValue tc;
		tc  = sobFactor * takeContrib(evalPatterns[Take], captured);
		score += tc;

		tc = sobFactor * threatContrib(evalPatterns[Threat], captured);
		score += tc;
	}

	UtilityValue bc;
	bc = evalPatterns[Blocked4] * _blockedFourBase;
	score += bc;

	// Give an advantage to having more pieces on one colour of squares
	// of a checkerboard
	bc = _posStats.getCheckerboardContrib(evalColour) * _checkerboardValue;
	score += bc;

	// Give an advantage to having more pieces on one colour of squares
	// of a horizontally or vertically striped board
	bc = _posStats.getStripeContrib(evalColour) * _stripeValue;
	score += bc;

#if 1
	// Give an advantage to having pieces closer together
	bc = _posStats.getSpreadContrib(evalColour) * _spreadValue;
	score += bc;
#if 0
	if (_spreadValue>0) {
		cout << "Spread val: " << _spreadValue << endl;
		cout << "bc: " << bc << endl;
	}
#endif
#endif

	return score;
}

// Captures become increasingly important as we approach 5
template <class PS>
UtilityValue UtilityCalc<PS>::capturedContrib(CapCount captures) const
{
	UtilityValue contrib = captures * _captureScoreBase * \
			_capturesScale[captures];
	return contrib;
}

template <class PS>
UtilityValue UtilityCalc<PS>::takeContrib(CapCount takes, CapCount captures) const
{
	// takes become increasingly important as we approach 5 captures
	UtilityValue contrib = takes * _takeScoreBase * \
			_capturesScale[captures];
	return contrib;
}

template <class PS>
UtilityValue UtilityCalc<PS>::threatContrib(CapCount threats, CapCount captures) const
{
	// threats become increasingly important as we approach 5 captures
	UtilityValue contrib = threats * _threatScoreBase * \
			_capturesScale[captures];
	return contrib;
}


template <class PS>
bool UtilityCalc<PS>::zeroTurnWin(Colour evalColour, Colour /*turnColour*/) const
{
	// Detect a win in this position
	CapCount evalCaptured = _posStats.getCaptured(evalColour);
	const PattCount *evalPatterns = _posStats.getPatternCounts(evalColour);

	if (evalPatterns[Line5] > 0)
	{
		// This position has been won already, mark it as such so
		// that the search is not continued from this node.
		// TODO: we shouldn't be modifying "state" here.
		//state.setWonBy(evalColour)
		return true;
	}

	// rules = self.rules
	// sfcw = rules.stonesForCaptureWin
	// ccp = rules.canCapturePairs

	// if evalCaptured >= sfcw:
	if (_posStats.getCanWinByCaptures() && evalCaptured >= 10)
	{
		// This position has been won already, mark it as such so
		// that the search is not continued from this node.
		// TODO: we shouldn't be modifying "state" here.
		// state.setWonBy(evalColour)
		return true;
	}

	return false;
}

template <class PS>
bool UtilityCalc<PS>::oneTurnWin(Colour evalColour, Colour turnColour) const
{
	// Detect a forceable win after one turn each
	CapCount evalCaptured = _posStats.getCaptured(evalColour);
	const PattCount *evalPatterns = _posStats.getPatternCounts(evalColour);

	if (evalPatterns[Line4] > 0)
	{
		if (evalColour == turnColour)
			// An unanswered line of four out of five will win
			return true;

		if (evalPatterns[Line4] > 1)
			// Two or more lines of four, with no danger of being
			// captured is a win.
			//if ccp:
			if (_posStats.getPatternCounts(otherPlayer(evalColour))[Take] == 0)
				return true;
	}

	if (_posStats.getCanWinByCaptures())
	{
		// Can win by captures
		CapCount myTakes = evalPatterns[Take];
		if (evalColour == turnColour)
		{
			if ((10 - evalCaptured) <= 2 and myTakes > 0)
				// evalColour can take the last pair for a win
				return true;
		} else {
			if ((10 - evalCaptured) <= 2 and myTakes > 2)
				// evalColour can take the last pair for a win
				return true;
		}
	}
	return false;
}

#endif
