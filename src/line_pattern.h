#ifndef _line_pattern_h
#define _line_pattern_h

#include "defines.h"

/*
 * A line pattern is used to match significant patterns in stretches of 5
 * Locs on the board. The LineLookupTable is used to find these rapidly.
 */

enum MatchType // TODO Rename to PatternType?
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

class LinePattern
{
public:
	LinePattern() : _colour(EMPTY), _matchType(NoMatch) {}

	bool operator ==(const LinePattern &other) const
	{ return (_colour == other._colour) &&
		     (_matchType == other._matchType) &&
			 (_candInds == other._candInds); }

	// For debugging only
	friend std::ostream & operator<<(std::ostream &os, const LinePattern& lti);

	Colour _colour;
	MatchType _matchType;
	vector<Breadth> _candInds;
};

#endif
