#ifndef _line_pattern_h
#define _line_pattern_h

#include "defines.h"

/*
 * A line pattern is used to match significant patterns in stretches of 5
 * Locs on the board. The LineLookupTable is used to find these rapidly.
 */

enum PatternType
{
	NoPattern=0,
	Line1=1,
	Line2=2,
	Line3=3,
	Line4=4,
	Threat=5,
	Take=6,
	Blocked=7,
	MAX_PATTERN_TYPE
};

class LinePattern
{
public:
	LinePattern() : _colour(EMPTY), _patternType(NoPattern) {}

	bool operator ==(const LinePattern &other) const
	{ return (_colour == other._colour) &&
		     (_patternType == other._patternType) &&
			 (_candInds == other._candInds); }

	// For debugging only
	friend std::ostream & operator<<(std::ostream &os, const LinePattern& lti);

	Colour _colour;
	PatternType _patternType;
	vector<Breadth> _candInds;
};

#endif
