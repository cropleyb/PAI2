#ifndef _board_reps_h
#define _board_reps_h

#include "loc.h"
#include "defines.h"

class PositionStats;
class SpanEntry;
class LinePattern;

class BoardReps
{
public:
	BoardReps(BoardWidth boardSize, PositionStats &posStats);

    void clear();

    void setOcc(Loc l, Colour c);
    Colour getOcc(Loc l) const;

private:
	void initEdges();
	void initEdgeVal(int dir, Loc l);

	PositionStats &_posStats;
	BoardWidth _boardSize;
	U64 _boardStrips[MAX_DIR][2*MAX_WIDTH];
};

#endif
