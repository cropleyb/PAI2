#ifndef _position_stats_h
#define _position_stats_h

#include "priority_level.h"

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

	void reportLengthCandidates(Colour colour, Length length, const vector<Loc> &locArr, Step inc);

	const PriorityLevel &getLengthPriorityLevel(Colour colour, Length length) const
	{
		return _levels[colour][length];
	}

	void reportCaptured(Colour c, CapCount count, int inc)
	{
		_captured[c] += count * inc;
	}

	CapCount getCaptured(Colour c) const
	{
		return _captured[c];
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

private:
	PriorityLevel _levels[3][6];
	CapCount _captured[3];
	PriorityLevel _takes[3];
	PriorityLevel _threats[3];
};

#endif
