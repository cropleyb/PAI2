#ifndef _board_reps_h
#define _board_reps_h

#include "loc.h"
#include "defines.h"

class PositionStats;

class BoardReps
{
public:
	BoardReps(PositionStats &posStats);

    void setOcc(Loc l, Colour c);
    Colour getOcc(Loc l);

private:
	PositionStats &_posStats;

	U64 _boardStrips[MAX_DIR][2*MAX_WIDTH];
};

#endif
