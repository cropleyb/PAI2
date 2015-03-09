
#include "defines.h"
#include "position_stats.h"
#include "line_pattern.h"
#include "span_entry.h"

// Test code only
void PositionStats::reportCandidate(Colour colour, PatternType pt, Loc loc, Step inc)
{
	PriorityLevel &level = _levels[colour][pt];
	level.addOrRemoveCandidate(loc, inc);
}

// Test code only
void PositionStats::reportCandidates(Colour colour, PatternType pt, const vector<Loc> &locArr, Step inc)
{
	PriorityLevel &level = _levels[colour][pt];
	for(Loc loc : locArr)
	{
		level.addOrRemoveCandidate(loc, inc);
	}
}

#include <assert.h>
#include <iostream>

// Convert each of the indices in patternEntry into Locs using span
// and update appropriately
void PositionStats::report(const SpanEntry &spanEntry, const LinePattern &patternEntry, int inc)
{
	Colour c = patternEntry._colour;
	int levelNum = (int)patternEntry._patternType;
	assert(levelNum < MAX_PATTERN_TYPE);
	PriorityLevel &level = _levels[c][(int)levelNum];
	for (Breadth b : patternEntry._candInds)
	{
		Loc loc = spanEntry.convertIndToLoc(b);
		std::cout << "Reporting loc" << loc._value << std::endl;
		level.addOrRemoveCandidate(loc, inc);
	}
}
