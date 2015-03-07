#ifndef _span_lookup_table_h
#define _span_lookup_table_h

#include "defines.h"
#include "loc.h"

class SpanEntry
{
public:
	BoardWidth _strip;    // Ind for strip number of a known direction
	BoardWidth _locIndex; // Index of this loc
    BoardWidth _minIndex; // Min index for updating this loc
	BoardWidth _maxIndex; // Max index for updating this loc
	Loc _baseLoc;         // Base location for this strip
	Loc _offsetPerIndex;  // Offset per index val for this strip
};

extern SpanEntry spanLookupTable[4][MAX_LOCS];

void buildSpanTable(BoardWidth boardSize);

#endif
