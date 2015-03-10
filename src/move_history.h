#ifndef _move_history_h
#define _move_history_h

#include "defines.h"
#include "loc.h"

class MoveHistory
{
public:
	MoveHistory();

    void saveMove(Loc move, Colour p, CaptureDirections cd);
};

#endif
