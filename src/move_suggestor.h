#include "defines.h"
#include "loc.h"

class PositionStats;
class CandidateCache;

class MoveSuggestor
{
public:
	MoveSuggestor(PositionStats &ps, CandidateCache &cc);

	Loc getNextMove(Depth d);

private:
	Breadth filterCandidates(Loc *moveBuffer, Depth d, Breadth maxMoves, Colour ourColour);

	PositionStats &_posStats;
	CandidateCache &_candCache;
};
