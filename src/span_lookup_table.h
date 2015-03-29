#ifndef _span_lookup_table_h
#define _span_lookup_table_h

#include "span_entry.h"
#include "defines.h"
#include "loc.h"

extern SpanEntry spanLookupTable[MAX_DIR][MAX_LOCS];

bool buildSpanTable(BoardWidth boardSize);

#endif
