#include "defines.h"
#include "loc.h"

#include <array>

class PositionStats;
class CandidateCache;
class PriorityLevel;

class MoveSuggester
{
public:
	MoveSuggester(PositionStats &ps);
	~MoveSuggester();

	void reset();

	Loc getNextMove(Depth d, Colour searchColour);
	bool isOnlyOneMove(Depth d, Colour searchColour);

private:
	bool getPriorityLevels(Colour ourColour);
	Breadth filterCandidates(Loc *moveBuffer, Depth d, Breadth maxMoves, Colour ourColour);

	void fillPriorityLevels(Colour ourColour, Colour theirColour);
	void fillCache(Depth depth, Colour searchColour);

	CandidateCache *_candCache;
	PositionStats &_posStats;
	
	std::array<const PriorityLevel *, MAX_CANDS> _toSearchLevels;
	int _numSearchLevels;
};
