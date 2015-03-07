#ifndef _board_reps_h
#define _board_reps_h

#include "loc.h"
#include "defines.h"
//#include "span_lookup_table.h"
//#include "line_lookup_table.h"

class PositionStats;
class SpanEntry;
class LineTableItem; // TODO Rename to PatternEntry

class BoardReps
{
public:
	BoardReps(PositionStats &posStats);

    void setOcc(Loc l, Colour c);
    Colour getOcc(Loc l);

private:
	void reportPattern(
		const LineTableItem *patternEntry, const SpanEntry *item, int inc);

	PositionStats &_posStats;

	U64 _boardStrips[MAX_DIR][2*MAX_WIDTH];
};

#endif
