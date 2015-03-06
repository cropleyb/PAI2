#ifndef _line_lookup_table_h
#define _line_lookup_table_h

#include "defines.h"

/*
 * The LineLookupTable maps lengths of 5 to significant pattern types.
 */

void buildAll();

typedef unsigned char Length;
typedef unsigned char Cands;

enum MatchType
{
	NoMatch=0,
	Line1=1,
	Line2=2,
	Line3=3,
	Line4=4,
	Threat=5,
	Take=6,
	Blocked=7,
	MAX_MATCH_TYPE
};

class LineTableItem
{
public:
	LineTableItem() : _colour(EMPTY), _matchType(NoMatch) {}
	bool operator ==(const LineTableItem &other) const
	{ return (_colour == other._colour) &&
		     (_matchType == other._matchType) &&
			 (_candInds == other._candInds); }

	Colour _colour;
	MatchType _matchType;
	vector<Breadth> _candInds;
};

const int MaxSpanMask = 4 * 4 * 4 * 4 * 4;

// The lengthLookup (and its contained class) is the only
// public scope variable, and only to LengthLookup.cpp (hopefully :) )
extern LineTableItem lengthLookup[MaxSpanMask];

#endif
