#ifndef _line_lookup_table_h
#define _line_lookup_table_h

#include "defines.h"
#include "line_pattern.h"

/*
 * The LineLookupTable maps lengths of 5 to significant pattern types.
 */

void buildLineLookupTable();

const int MaxSpanMask = 4 * 4 * 4 * 4 * 4;

// The lengthLookup (and its contained class) is the only
// public scope variable, and only to LengthLookup.cpp (hopefully :) )
extern LinePattern lengthLookup[MaxSpanMask];
// extern std::array<LinePattern, MaxSpanMask> lengthLookup; // TODO
// Threats are usually also Line2s - this is a secondary table for the
// original Line2s.
extern LinePattern threatLookup[MaxSpanMask];

#endif
