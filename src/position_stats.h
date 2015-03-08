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

	// Test code only?
	void reportCandidate(Colour colour, PatternType pt, Loc loc, Step inc);
	void reportCandidates(Colour colour, PatternType pt, const vector<Loc> &locArr, Step inc);
#if 0

	const PriorityLevel &getLengthPriorityLevel(Colour colour, Length length) const
	{
		return _levels[colour][length];
	}

	void reportTake(Colour c, Loc l, int inc)
	{
		_takes[c].addOrRemoveCandidate(l, inc);
	}

	const PriorityLevel &getTakesPriorityLevel(Colour colour) const
	{
		return _takes[colour];
	}

	void reportThreat(Colour c, Loc l, int inc)
	{
		_threats[c].addOrRemoveCandidate(l, inc);
	}

	const PriorityLevel &getThreatsPriorityLevel(Colour colour) const
	{
		return _threats[colour];
	}
#endif
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
	PriorityLevel _levels[3][10];
	CapCount _captured[3];
};

#endif
