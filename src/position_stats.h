#ifndef _position_stats_h
#define _position_stats_h

#include "priority_level.h"
#include "defines.h"
#include "line_pattern.h"

#include <iostream>

class SpanEntry;
//class LinePattern;

class PositionStats
{
public:
	PositionStats()
	{
		_canWinByCaptures = true;
		reset();
	}

	void reset()
	{
		for (int c=0; c<3; c++)
		{
			_captured[c] = 0;
			for (int p=0; p<MAX_PATTERN_TYPE; p++)
			{
				_patternCounts[c][p] = 0;
				_levels[c][p].reset();
			}
			_checkerboardStats[c][0] = 0;
			_checkerboardStats[c][1] = 0;
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

	const PriorityLevel &getPriorityLevel(Colour c, PatternType pattern) const
	{
		return _levels[c][pattern];
	}

	const PattCount *getPatternCounts(Colour c) const
	{
		return _patternCounts[c];
	}
	
	PattCount getNumPatterns(Colour c, PatternType pattern) const
	{
		return _patternCounts[c][pattern];
	}

	Colour getWonBy() const
	{
		return _wonBy;
	}

	void updateCheckerboardStats(Colour c, Loc loc, int inc)
	{
        if (!c)
            // We don't care about empty spaces
            return;

        Colour squareColour = (loc[0] % 2) ^ (loc[1] % 2);

        _checkerboardStats[c][squareColour] += inc;
	}

	UtilityValue getCheckerboardContrib(Colour c) const
	{
		const short *ours = _checkerboardStats[c];
		
		UtilityValue ret = abs(ours[0] - ours[1])/(ours[0] + ours[1] + 1);
		return ret;
	}

	void setCanWinByCaptures(bool c) { _canWinByCaptures = c; }
	bool getCanWinByCaptures() const { return _canWinByCaptures; }

private:
	void updateWonBy(Colour c)
	{
		_wonBy = EMPTY;
		if (_canWinByCaptures && _captured[c] >= 10) _wonBy = c;
		else if (_patternCounts[c][Line5] > 0) _wonBy = c;
	}

	// Test code only
	void reportCandidate(Colour colour, PatternType pt, Loc loc, Step inc);
	void reportCandidates(Colour colour, PatternType pt, const vector<Loc> &locArr, Step inc);

	// TODO: Extract player stats?
	PriorityLevel _levels[3][MAX_PATTERN_TYPE];
	PattCount _patternCounts[3][MAX_PATTERN_TYPE];
	CapCount _captured[3];
	short _checkerboardStats[3][2];
	Colour _wonBy;
	bool _canWinByCaptures;
};

#endif
