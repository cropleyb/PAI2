#ifndef _position_stats_h
#define _position_stats_h

#include "priority_level.h"
#include "defines.h"
#include "line_pattern.h"
#include "cap_table.h"
#include "special_occs_table.h"

#include <iostream>
#include <cmath>

class SpanEntry;

class PositionStats
{
public:
	PositionStats()
	{
		//_takeTargeting = false;
		_takeTargeting = true;
		_canWinByCaptures = true;
		reset();
	}

	void reset()
	{
		for (int c=0; c<3; c++)
		{
			_captured[c] = 0;
			_trias[c] = 0;
			_potentials[c] = 0;

#if 0
			// For calculating spread
			_totalStones[c] = 0.001; // div by zero prevention
			_totalDistSqr[c] = 0;
			_totalDist[c] = 0;
#endif

			for (int p=0; p<MAX_PATTERN_TYPE; p++)
			{
				_patternCounts[c][p] = 0;
				_levels[c][p].reset();
				_levels[c][p].setLevelName(string("P")+to_string(c)+"-"+to_string(p));
			}
			for (int q=0; q<2; q++)
			{
				_checkerboardStats[c][q] = 0;
				for (int r=0; r<2; r++)
				{
					_stripeStats[c][q][r] = 0;
				}
			}
		}
		_wonBy = EMPTY;
	}

	void reportCaptured(Colour c, CapCount count, int inc)
	{
		_captured[c] += count * inc;
		updateWonBy(c);
		// std::cout << " -> " << (int)_captured[c];
	}

	CapCount getCaptured(Colour c) const
	{
		return _captured[c];
	}

	void report(const SpanEntry &spanEntry, const LinePattern &patternEntry, int inc);
	void maintainTakePLs(const SpanEntry &spanEntry, const LinePattern &patternEntry, int inc);
	void maintainSpecial(const SpanEntry &spanEntry, const LinePattern &patternEntry, int inc);
	void maintainTake(const SpanEntry &spanEntry, const LinePattern &patternEntry, int inc);

	const PriorityLevel &getPriorityLevel(Colour c, int pattern) const
	{
		return _levels[c][pattern];
	}

	const PattCount *getPatternCounts(Colour c) const
	{
		return _patternCounts[c];
	}
	
	PattCount getNumPatterns(Colour c, int patternType) const
	{
		return _patternCounts[c][patternType];
	}

	Colour getWonBy() const
	{
		return _wonBy;
	}

	Breadth getMovesToWin(Colour c)
	{
		if (getNumPatterns(c, Line4) > 0) {
			return 1;
		}
		if (_captured[c] >= 8 && getNumPatterns(c, Take) > 0) {
			return 1;
		}
		if (getNumPatterns(c, Line3) > 0) {
			return 2;
		}
		if (getNumPatterns(c, Blocked4Take) > 1) {
			// c has a blocked four and a take of the blocker -> 2
			return 2;
		}
		if (_captured[c] >= 8 && getNumPatterns(c, Threat) > 0) {
			return 2;
		}
		if (_captured[c] >= 6 && getNumPatterns(c, Take) > 1) {
			// Guess that there will be enough for two caps. i.e. no shared stones in the takes
			return 2;
		}
		// TODO: skip VCT earlier in the game...
		// TODO: VCT > 3
		return 3;
	}

	Breadth getMultiCount(Colour c, int level) const {
		return _levels[c][level].getMultiCount();
	}

#if 1
	void updateSpreadCounts(Colour c, Loc loc, int inc)
	{
		_totalStones[c] += inc;
		float x = loc[0];
		float y = loc[1];
		float distSqr = x*x + y*y;
		//cout << "distSqr: " << distSqr << endl;
		_totalDistSqr[c] += distSqr;
		// TODO: Use a lookup table for sqrt.
		_totalDist[c] += sqrt(distSqr);
		//cout << "_totalDist: " << _totalDist[c] << endl;
	}
#endif

	void updateStrategicStats(Colour c, Loc loc, int inc)
	{
        if (!c)
            // We don't care about empty spaces
            return;

        int stripe[2];
	    stripe[0] =	(loc[0] % 2);
	    stripe[1] =	(loc[1] % 2);

        Colour squareColour;
        squareColour = (stripe[0]) ^ (stripe[1]);
        _checkerboardStats[c][squareColour] += inc;

        _stripeStats[c][0][stripe[0]] += inc;
        _stripeStats[c][1][stripe[1]] += inc;

#if 1
		updateSpreadCounts(c, loc, inc);
#endif
	}

	UtilityValue getCheckerboardContrib(Colour c) const
	{
		const UtilityValue *ours = _checkerboardStats[c];
		UtilityValue absval = ours[0] - ours[1];
		if (absval < 0) absval = -absval;
		
		UtilityValue ret = absval/(ours[0] + ours[1] + 1);
		return ret;
	}

	UtilityValue getStripeContrib(Colour c) const
	{
		UtilityValue ret;
		ret = getStripeContribPerDir(c, false);
		ret += getStripeContribPerDir(c, true);
		return ret;
	}

	UtilityValue getStripeContribPerDir(Colour c, bool vertical) const
	{
		const UtilityValue *ours = _stripeStats[c][vertical];
		UtilityValue absval = ours[0] - ours[1];
		if (absval < 0) absval = -absval;
		
		UtilityValue ret = absval/(ours[0] + ours[1] + 1);
		return ret;
	}

#if 1
	UtilityValue getSpreadContrib(Colour c) const
	{
		float diff = (_totalDist[c] * _totalDist[c]) - (_totalStones[c] * _totalDistSqr[c]);
		//cout << "diff: " << diff << endl;
		UtilityValue stddev = sqrt(diff) / _totalStones[c];
		//cout << "stddev: " << stddev << endl;
		return stddev;
	}
#endif

	float getStonesOnBoard(Colour c) const { return _totalStones[c]; }

	PattCount getTriaCount(Colour c) const { return _trias[c]; }
	PattCount getPotentialCount(Colour c) const { return _potentials[c]; }

	void setCanWinByCaptures(bool c) { _canWinByCaptures = c; }
	bool getCanWinByCaptures() const { return _canWinByCaptures; }

	bool isForced(Colour currentPlayer) { return getMovesToWin(currentPlayer) > getMovesToWin(otherPlayer(currentPlayer)); }

	bool _takeTargeting;

private:
	void updateWonBy(Colour c)
	{
		_wonBy = EMPTY;
		if (_canWinByCaptures && _captured[c] >= 10) _wonBy = c;
		else if (_patternCounts[c][Line5] > 0) _wonBy = c;
	}

	// Test code only
	void reportCandidate(Colour colour, LinePatternType pt, Loc loc, Step inc);
	void reportCandidates(Colour colour, LinePatternType pt, const vector<Loc> &locArr, Step inc);
	void reportCandidate(Colour colour, ExtraPriorityLevels pt, Loc loc, Step inc);

	// TODO: Extract player stats?
	PriorityLevel _levels[3][MAX_PATTERN_TYPE];
	PattCount _patternCounts[3][MAX_PATTERN_TYPE];
	CapCount _captured[3];

	// Strategic features
	UtilityValue _checkerboardStats[3][2];
	UtilityValue _stripeStats[3][2][2];

	PattCount _trias[3];
	PattCount _potentials[3];

#if 1
	// for Spread
	float _totalStones[3];
	float _totalDistSqr[3];
	float _totalDist[3];
#endif

	Colour _wonBy;
	bool _canWinByCaptures;

	CapTable _takeTable[3]; // 2 colours
	SpecialOccsTable _specialOccsTable[3]; // 2 colours
	// StId _structureLookup[MAX_PATTERN_TYPE][MAX_LOCS];
};

#endif
