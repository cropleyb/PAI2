#include "defines.h"
#include "loc.h"

class PositionStats;
class CandidateCache;
class PriorityLevel;

class MoveSuggester
{
public:
	MoveSuggester(PositionStats &ps);
	~MoveSuggester();

	Loc getNextMove(Depth d);

private:
	bool getPriorityLevels(Colour ourColour);
	Breadth filterCandidates(Loc *moveBuffer, Depth d, Breadth maxMoves, Colour ourColour);

	void fillPriorityLevels(Colour ourColour, Colour theirColour);

	CandidateCache *_candCache;
	PositionStats &_posStats;
	
	// This could be on the stack really
	const PriorityLevel *_toSearchLevels[MAX_CANDS];
	int _numSearchLevels;
};
