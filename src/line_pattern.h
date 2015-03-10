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
	Threat=3,
	Line3=4,
	Take=5,
	Line4=6,
	Blocked=7,
	MAX_PATTERN_TYPE
};

class LinePattern
{
public:
	LinePattern() : _colour(EMPTY), _patternType(NoPattern) {}

	LinePattern(const LinePattern &orig) :
		_colour(orig._colour),
		_patternType(orig._patternType),
	    _candInds(orig._candInds)
	{}

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
