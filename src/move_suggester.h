#include "defines.h"
#include "loc.h"

class PositionStats;
class CandidateCache;
//class PriorityLevel;

#include "priority_level.h"

class MoveSuggester
{
public:
	MoveSuggester(PositionStats &ps, CandidateCache &cc);

	Loc getNextMove(Depth d);

private:
	bool getPriorityLevels(Colour ourColour);
	Breadth filterCandidates(Loc *moveBuffer, Depth d, Breadth maxMoves, Colour ourColour);

	void fillPriorityLevels(Colour ourColour, Colour theirColour);

	PositionStats &_posStats;
	CandidateCache &_candCache;
	
	// This could be on the stack really
	const PriorityLevel *_toSearchLevels[MAX_CANDS];
	int _numSearchLevels;
};
