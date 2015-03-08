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
		// TODO Ugly as.
		_captured[0] = 0;
		_captured[1] = 0;
		_captured[2] = 0;
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
	
private:
	// Test code only
	void reportCandidate(Colour colour, PatternType pt, Loc loc, Step inc);
	void reportCandidates(Colour colour, PatternType pt, const vector<Loc> &locArr, Step inc);

	PriorityLevel _levels[3][10];
	CapCount _captured[3];
};

#endif
