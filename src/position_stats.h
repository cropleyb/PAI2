#ifndef _position_stats_h
#define _position_stats_h

#include "priority_level.h"

class PositionStats
{
public:
	void reportLengthCandidates(Colour colour, Length length, const vector<Loc> &locArr, Step inc);

	const PriorityLevel &getLengthPriorityLevel(Colour colour, Length length) const
	{
		return _levels[colour][length];
	}

private:
	PriorityLevel _levels[3][6];
};

#endif
