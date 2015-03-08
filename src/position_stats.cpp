
#include "defines.h"
#include "position_stats.h"
#include "line_pattern.h"
#include "span_lookup_table.h" // TODO: separate out span_entry

// Testing only
void PositionStats::reportCandidate(Colour colour, PatternType pt, Loc loc, Step inc)
{
	PriorityLevel &level = _levels[colour][pt];
	level.addOrRemoveCandidate(loc, inc);
}

// Testing only
void PositionStats::reportCandidates(Colour colour, PatternType pt, const vector<Loc> &locArr, Step inc)
{
	PriorityLevel &level = _levels[colour][pt];
	for(Loc loc : locArr)
	{
		level.addOrRemoveCandidate(loc, inc);
	}
}

void PositionStats::report(const SpanEntry &spanEntry, const LinePattern &patternEntry, int inc)
{
	Colour c = patternEntry._colour;
	int levelNum = (int)patternEntry._patternType;
	PriorityLevel &level = _levels[c][(int)levelNum];
	for (Breadth b : patternEntry._candInds)
	{
		Loc loc = spanEntry.convertIndToLoc(b);
		level.addOrRemoveCandidate(loc, inc);
	}
	    //vector<Breadth> _candInds;
	// Convert each of the indices in patternEntry into Locs using span
	// and report appropriately
}
