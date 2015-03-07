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

#include <sstream>
using std::string;
using std::endl;
using std::ostringstream;

class LinePattern
{
public:
	LinePattern() : _colour(EMPTY), _matchType(NoMatch) {}

	bool operator ==(const LinePattern &other) const
	{ return (_colour == other._colour) &&
		     (_matchType == other._matchType) &&
			 (_candInds == other._candInds); }

	// For debugging only
    string to_str() const;
	friend std::ostream & operator<<(std::ostream &os, const LinePattern& lti);

	Colour _colour;
	MatchType _matchType;
	vector<Breadth> _candInds;
};

const int MaxSpanMask = 4 * 4 * 4 * 4 * 4;

// The lengthLookup (and its contained class) is the only
// public scope variable, and only to LengthLookup.cpp (hopefully :) )
extern LinePattern lengthLookup[MaxSpanMask];
// Threats are usually also Line2s - this is a secondary table for the
// original Line2s.
extern LinePattern threatLookup[MaxSpanMask];

#endif
