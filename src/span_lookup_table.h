#ifndef _span_lookup_table_h
#define _span_lookup_table_h

#include "defines.h"
#include "loc.h"

class SpanEntry
{
public:
	// DSIndex _dsInd; should know this already in caller code?
    BoardWidth _minIndex;
	BoardWidth _maxIndex;
	BoardWidth _locIndex;
	Loc _baseLoc;
	Loc _offsetPerIndex;
};

extern SpanEntry spanLookupTable[4][MAX_LOCS];

void buildSpanTable(BoardWidth boardSize);

#endif
