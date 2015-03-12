#ifndef _position_stats_h
#define _position_stats_h

#include "priority_level.h"
#include "defines.h"
#include "line_pattern.h"

class SpanEntry;
//class LinePattern;

class PositionStats
{
public:
	PositionStats()
	{
		for (int c=0; c<3; c++)
		{
			_captured[c] = 0;
			for (int p=0; p<MAX_PATTERN_TYPE; p++)
			{
				_patternCounts[c][p] = 0;
			}
		}
	}

	void reportCaptured(Colour c, CapCount count, int inc)
	{
		_captured[c] += count * inc;
	}

	CapCount getCaptured(Colour c) const
	{
		return _captured[c];
	}


	// TODO: checkerboardStats
	
	void report(const SpanEntry &spanEntry, const LinePattern &patternEntry, int inc);

	const PriorityLevel &getPriorityLevel(Colour c, PatternType pattern) const
	{
		return _levels[c][pattern];
	}
	
	PattCount getNumPatterns(Colour c, PatternType pattern) const
	{
		return _patternCounts[c][pattern];
	}

private:
	// Test code only
	void reportCandidate(Colour colour, PatternType pt, Loc loc, Step inc);
	void reportCandidates(Colour colour, PatternType pt, const vector<Loc> &locArr, Step inc);

	PriorityLevel _levels[3][MAX_PATTERN_TYPE];
	PattCount _patternCounts[3][MAX_PATTERN_TYPE];
	CapCount _captured[3];
};

#endif
