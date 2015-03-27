
#include "defines.h"
#include "position_stats.h"
#include "line_pattern.h"
#include "span_entry.h"

// Test code only
void PositionStats::reportCandidate(Colour colour, PatternType pt, Loc loc, Step inc)
{
	_patternCounts[colour][pt] += inc;

	PriorityLevel &level = _levels[colour][pt];
	level.addOrRemoveCandidate(loc, inc);
}

// Test code only
void PositionStats::reportCandidates(Colour colour, PatternType pt, const vector<Loc> &locArr, Step inc)
{
	_patternCounts[colour][pt] += inc;

	PriorityLevel &level = _levels[colour][pt];
	for(Loc loc : locArr)
	{
		level.addOrRemoveCandidate(loc, inc);
	}
}

//#include <assert.h>

// Convert each of the indices in patternEntry into Locs using span
// and update appropriately
void PositionStats::report(const SpanEntry &spanEntry, const LinePattern &patternEntry, int inc)
{
	Colour c = patternEntry._colour;
	int levelNum = (int)patternEntry._patternType;
	//assert(levelNum < MAX_PATTERN_TYPE);

	_patternCounts[c][levelNum] += inc;
	if (levelNum == Line5) updateWonBy(c);

	PriorityLevel &level = _levels[c][levelNum];
	for (int i=0; i<patternEntry._numInds; i++)
	{
		Loc loc = spanEntry.convertIndToLoc(patternEntry._candInds[i]);
		level.addOrRemoveCandidate(loc, inc);
	}
}
