
#include "defines.h"
#include "position_stats.h"
#include "line_pattern.h"
#include "span_entry.h"

// Test code only
void PositionStats::reportCandidate(Colour colour, LinePatternType pt, Loc loc, Step inc)
{
	_patternCounts[colour][pt] += inc;

	PriorityLevel &level = _levels[colour][pt];
	level.addOrRemoveCandidate(loc, inc);
}

// Test code only
void PositionStats::reportCandidates(Colour colour, LinePatternType pt, const vector<Loc> &locArr, Step inc)
{
	_patternCounts[colour][pt] += inc;

	PriorityLevel &level = _levels[colour][pt];
	for(Loc loc : locArr)
	{
		level.addOrRemoveCandidate(loc, inc);
	}
}

//#include <assert.h>

void PositionStats::maintainTakePLs(const SpanEntry &spanEntry, const LinePattern &patternEntry, int inc)
{
	Colour c = patternEntry._colour;
	LinePatternType pt = patternEntry._patternType;

	switch (pt) {
		case Line4:
			// _specialOccs.add(Loc trigger, DirectionType dir, int inc);
			break;
		case Blocked4:
			// _specialOccs.add(Loc trigger, DirectionType dir, int inc);
			break;
		case Take:
			// _takeTable.addCap(Loc trigger, DirectionType dir, int inc);
			// _specialOccs.add(Loc trigger, DirectionType dir, int inc);
			break;
		default:
			break;
	}
}

// Convert each of the indices in patternEntry into Locs using span,
// and update appropriately
void PositionStats::report(const SpanEntry &spanEntry, const LinePattern &patternEntry, int inc)
{
	Colour c = patternEntry._colour;
	LinePatternType pt = patternEntry._patternType;
	//assert(pt < MAX_LINE_PATTERN_TYPE);

	if (_takeTargeting) {
		maintainTakePLs(spanEntry, patternEntry, inc);
	}

#if 0
	StId stId = -1;

    if (pt == Line4 or pt == Blocked4 or pt == Line3)
    {
        if (inc > 0) {
			Structure &str = _structureMap.add(pt);
			str._pattern = pt;
			stId = str._id;
			_structureLookup[pt][spanEntry._baseLoc] = stId;
		} else {
			// TODO _structureLookup needs to include direction as well
	// spanEntry._direction; // Direction that this strip goes - up to MAX_DIR (i.e. 4)
			stId = _structureLookup[pt][spanEntry._baseLoc];
			_structureMap.remove(stId);
		}
	}
#endif

	_patternCounts[c][pt] += inc;
	if (pt == Line5) updateWonBy(c);

	PriorityLevel &level = _levels[c][pt];
	for (int i=0; i<patternEntry._numCands; i++)
	{
		Loc loc = spanEntry.convertIndToLoc(patternEntry._inds[i]);
		level.addOrRemoveCandidate(loc, inc);
	}
}

