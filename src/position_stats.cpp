
#include "defines.h"
#include "position_stats.h"

void PositionStats::reportLengthCandidates(Colour colour, Length length, const vector<Loc> &locArr, Step inc)
{
	PriorityLevel &level = _levels[colour][length];
	for(Loc loc : locArr)
	{
		level.addOrRemoveCandidate(loc, inc);
	}
}
