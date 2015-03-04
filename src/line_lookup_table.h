#ifndef _length_lookup_table_h
#define _length_lookup_table_h

#include "defines.h"

void buildAll();

typedef unsigned char Length;
typedef unsigned char Cands;

enum MatchType
{
	None=0,
	Line=1,
	Threat=2,
	Take=3,
	Blocked=4,
	MAX_MATCH_TYPE
};

class LineTableItem
{
public:
	LineTableItem() : _colour(EMPTY), _length(0) {}

	Colour _colour;
	Length _length;
	MatchType _matchType;
	vector<Breadth> _candInds;
};

const int MaxSpanMask = 4 * 4 * 4 * 4 * 4;

// The lengthLookup (and its contained class) is the only
// public scope variable, and only to LengthLookup.cpp (hopefully :) )
extern LineTableItem lengthLookup[MaxSpanMask];

#endif
